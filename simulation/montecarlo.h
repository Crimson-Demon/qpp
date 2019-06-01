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
        rnd::GenPtr generator;
        unsigned long seed;
        uint64_t paths;
        double alpha;

    public:
        RunSettings() {
            type = RunType::SEQUENTIAL;
            generator = rnd::GenPtr(rnd::GeneratorFactory<std::mt19937>::get());
            seed = 69;
            paths = 10000;
            alpha = 0.95;
        }

        RunSettings(RunType type, rnd::BaseGenerator *g, unsigned long seed, uint64_t nPaths, double alpha) :
                type(type), generator(g), seed(seed), paths(nPaths), alpha(alpha) {}

        RunType get_type() const { return type; }

        rnd::GenPtr get_generator() const { return generator; }

        unsigned long get_seed() const { return seed; }

        uint64_t get_paths() const { return paths; }

        double get_alpha() const { return alpha; }
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

        double get_value() const { return value; }

        double get_lower_CI() const { return lowerCI; }

        double get_upper_CI() const { return upperCI; }

        std::chrono::duration<long int, std::ratio<1, 1000>> get_duration() const { return duration; }

        std::string to_string() const {
            std::string s;
            s += "{MCResult: {value: " + std::to_string(value) + "}, lowerCI: " + std::to_string(lowerCI) +
                 "}, upperCI: " + std::to_string(upperCI) + "}, duration: " + std::to_string(duration.count()) + "}}";
            return s;
        }

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
            unsigned paths = settings.get_paths();
            auto start = std::chrono::steady_clock::now();
            auto seed = settings.get_seed();
            auto generator = settings.get_generator();
            generator->seed(seed);
            switch (settings.get_type()) {
                case RunType::CONCURRENT:
                    runResults = concurrent_run(job, generator.get(), paths);
                default:
                    runResults = sequential_run(job, generator.get(), paths);
            }
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start);

            double alpha = settings.get_alpha();
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
        std::vector<double> static sequential_run(MonteCarloJob &job, rnd::BaseGenerator *g, uint64_t nPaths) {
            std::vector<double> results;
            double localResult;
            for (uint64_t i = 0; i < nPaths; ++i) {
                localResult = job.run(g);
                results.push_back(localResult);
            }
            return results;
        }

        std::vector<double> static concurrent_run(MonteCarloJob &job, rnd::BaseGenerator *g, uint64_t nPaths) {
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