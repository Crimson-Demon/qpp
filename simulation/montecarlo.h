#ifndef SIMLIB_MONTECARLO_H
#define SIMLIB_MONTECARLO_H

#include <functional>
#include <future>
#include <vector>
#include "random.h"

namespace mc {
    enum class RunType {
        CONCURRENT,
        SEQUENTIAL
    };

    class RunSettings {
        RunType type;
        rnd::BaseGenerator *generator;
        uint64_t nPaths;
        double alpha;

    public:
        RunSettings(RunType type, rnd::BaseGenerator *g, uint64_t nPaths, double alpha) : type(type), generator(g),
                                                                                          nPaths(nPaths),
                                                                                          alpha(alpha) {}

        RunType getType() const { return type; }

        rnd::BaseGenerator *getGenerator() const { return generator; }

        uint64_t getPaths() const { return nPaths; }

        double getAlpha() const { return alpha; }
    };

    class MonteCarloResult {
        double value;
        double lowerCI;
        double upperCI;
        std::chrono::duration<long int, std::ratio<1, 1000>> duration;
    public:
        explicit MonteCarloResult(double value, double lowerCI, double upperCI,
                                  std::chrono::duration<long int, std::ratio<1, 1000>> duration) :
                value(value), lowerCI(lowerCI), upperCI(upperCI), duration(duration) {}

        double getValue() const { return value; }

        double getLowerCI() const { return lowerCI; }

        double getUpperCI() const { return upperCI; }

        std::chrono::duration<long int, std::ratio<1, 1000>> getDuration() const { return duration; }

    };

    class MonteCarloJob {
        std::function<double(rnd::BaseGenerator *)> callback;
    public:
        explicit MonteCarloJob(std::function<double(rnd::BaseGenerator *)> &callback) : callback(std::move(callback)) {}

        double run(rnd::BaseGenerator *g) const { return callback(g); };
    };

    class MonteCarloEngine {
    public:
        static MonteCarloResult run(MonteCarloJob &job, RunSettings &settings) {
            std::vector<double> runResults;
            unsigned paths = settings.getPaths();
            auto start = std::chrono::steady_clock::now();
            switch (settings.getType()) {
                case RunType::CONCURRENT:
                    runResults = concurrentRun(job, settings.getGenerator(), paths);
                default:
                    runResults = sequentialRun(job, settings.getGenerator(), paths);
            }
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start);

            double alpha = settings.getAlpha();
            double a = std::accumulate(runResults.begin(), runResults.end(), 0.0) / paths;
            double b2 = std::accumulate(runResults.begin(), runResults.end(), 0.0,
                                        [a](double acc, double val) -> double { return acc + std::pow(val - a, 2); }) /
                        (paths - 1);
            double z = rnd::NormalDistribution::inv_cdf(1 - alpha / 2, 0.0, 1.0);
            double c1 = a - z * std::sqrt(b2 / paths), c2 = a + z * std::sqrt(b2 / paths);

            MonteCarloResult result(a, c1, c2, duration);

            return result;
        }

    private:
        std::vector<double> static sequentialRun(MonteCarloJob &job, rnd::BaseGenerator *g, uint64_t nPaths) {
            std::vector<double> results;
            double localResult;
            for (uint64_t i = 0; i < nPaths; ++i) {
                localResult = job.run(g);
                results.push_back(localResult);
            }
            return results;
        }

        std::vector<double> static concurrentRun(MonteCarloJob &job, rnd::BaseGenerator *g, uint64_t nPaths) {
            std::future<double> jobResults[nPaths];
            double jobResult;
            std::vector<double> results;

            unsigned i = 0;
            while (i < nPaths) {
                unsigned j;
                for (j = 0; j < nPaths && i < nPaths; ++j, ++i) {
                    jobResults[j] = std::async([&job, g]() -> double { return job.run(g); });
                }
                for (unsigned k = 0; k < j; ++k) {
                    jobResult = jobResults[k].get();
                    results.push_back(jobResult);
                }
            }
            return results;
        }
    };
}

#endif //SIMLIB_MONTECARLO_H