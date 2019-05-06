#ifndef SIMLIB_SIMULATIONUTILS_H
#define SIMLIB_SIMULATIONUTILS_H

#include <functional>
#include <future>
#include "random.h"

namespace qpp {
    enum class RunType {
        CONCURRENT,
        SEQUENTIAL
    };

    class RunSettings {
        RunType type;
        BaseGenerator *generator;
        uint64_t nPaths;

    public:
        RunSettings(RunType type, BaseGenerator *g, uint64_t nPaths) : type(type), generator(g), nPaths(nPaths) { }
        RunType getType() const { return type; }
        BaseGenerator* getGenerator() const { return generator; }
        uint64_t getPaths() const { return nPaths; }
    };

//    template <typename ResultType>
    class MonteCarloJob {
        std::function<double(BaseGenerator*)> callback;
    public:
        explicit MonteCarloJob(std::function<double(BaseGenerator*)>& callback) : callback(std::move(callback)) { }
        double compute(BaseGenerator *g) const { return callback(g); };
    };

    class MonteCarloEngine {
    public:
        double static run(MonteCarloJob &job, RunSettings &settings) {
            switch(settings.getType()) {
                case RunType::CONCURRENT:
                    return concurrentRun(job, settings.getGenerator(), settings.getPaths());
                case RunType::SEQUENTIAL:
                    return sequentialRun(job, settings.getGenerator(), settings.getPaths());
            }
        }
    private:
        double static sequentialRun(MonteCarloJob &job, BaseGenerator *g, uint64_t nPaths) {
            std::vector<double> results;
            double localResult, result = 0.0;
            for(uint64_t i = 0; i < nPaths; ++i) {
                localResult = job.compute(g);
                results.push_back(localResult);
                result += localResult;
            }
            return result / (double) nPaths;
        }
        double static concurrentRun(MonteCarloJob &job, BaseGenerator *g, uint64_t nPaths) {
            std::future<double> jobResults[nPaths];
            double result = 0.0;
            double jobResult;
            std::vector<double> results;

            unsigned i = 0;
            while(i < nPaths) {
                unsigned j;
                for(j = 0; j < nPaths && i < nPaths; ++j, ++i) {
                    jobResults[j] = std::async( [&job, g]()->double{ return job.compute(g); } );
                }
                for(unsigned k = 0; k < j; ++k) {
                    jobResult = jobResults[k].get();
                    results.push_back(jobResult);
                    result += jobResult;
                }
            }
            return result / (double) nPaths;
        }
    };
}

#endif //SIMLIB_SIMULATIONUTILS_H
