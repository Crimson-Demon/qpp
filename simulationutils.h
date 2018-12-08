#ifndef QPP_SIMULATIONUTILS_H
#define QPP_SIMULATIONUTILS_H

#include <random>
#include <functional>
#include <future>

namespace qpp {
    class BaseGenerator {
    public:
        virtual uint operator()() = 0;
        virtual void seed() = 0;
        // todo: those should be static constexpr - check if they wont break
        virtual uint min() = 0;
        virtual uint max() = 0;
    };

    // todo: add seeding logic
    template <typename Generator>
    class GenericGenerator {
        Generator generator;
    public:
        GenericGenerator(Generator &g): generator(g) { }
        uint operator()() {
            return generator();
        }
        void seed() {
            generator.seed();
        }
        uint min() {
            return generator.min();
        }
        uint max() {
            return generator.max();
        }
    };

    // todo: cannot use double/float as template classes, look for reasonable workaround to template with double
    class NormalDistribution {
        double mean;
        double variance;
        std::normal_distribution<double> distribution;
//        std::normal_distribution<double> stdDistribution;
    public:
        NormalDistribution(double mean, double variance): mean(mean), variance(variance), distribution(std::normal_distribution<double>(mean, variance))/*, stdDistribution(std::normal_distribution<double>(0.0, 1.0))*/ { }
        // todo: we really need a better approximation for normcdf than erf!!
        static constexpr double std_cdf(double value) { return cdf(value, 0.0, 1.0); }
        static constexpr double cdf(double value, double mean, double variance) { return 0.5 * (1 + erf((value - mean) / (variance * (M_SQRT1_2)))); }
        double cdf(double value) { return cdf(value, mean, variance); }
        static double rand(BaseGenerator *g, double mean, double variance) {
            std::normal_distribution<double> localDistribution (mean, variance);
            // todo: does this actually work?
            return localDistribution(*g);
        }
        double rand(BaseGenerator *g) {
            // todo: does this actually work?
            return distribution(*g);
        }
    };

    // todo: maybe make the methods static where possible?
    class SDE {
    public:
//        virtual std::function<double(BaseGenerator*)> solution(double x, double t) = 0;
        virtual double solve(BaseGenerator *g, double x, double t) = 0;
//        virtual double quadraticVariation(double x, double t) = 0;
        virtual double drift(double x, double t) = 0;
        virtual double diffusion(double x, double t) = 0;
    };

    // todo: maybe make the methods static where possible?
    class GBM : public SDE {
        double mu;
        double sigma;
    public:
        GBM(double mu, double sigma) : mu(mu), sigma(sigma) { }

//        std::function<double(BaseGenerator*)> solution(double x, double t) {
//            return [x, t, mu, sigma](BaseGenerator* g)->double{ return x * std::exp((mu - std::pow(sigma, 2) * 0.5) * t + sigma * NormalDistribution::rand(g, 0.0, t)); };
//        }

        double solve(BaseGenerator *g, double x, double t) {
            return x * std::exp((mu - std::pow(sigma, 2) * 0.5) * t + sigma * NormalDistribution::rand(g, 0.0, t));
        }

        // todo:
//        double quadraticVariation(double x, double t) {
//            return std::pow(sigma * x, 2);
//        }

        double drift(double x, double t) {
            return mu * x;
        }

        double diffusion(double x, double t) {
            return sigma * x;
        }
    };

    class SimulationScheme/* : public ComputationScheme*/ {
    public:
        virtual double trajectory(SDE *sde, BaseGenerator *g, double x0, double T, uint64_t steps) = 0;
    };

    class DirectSimulationScheme : public SimulationScheme {
    public:
        double trajectory(SDE *sde, BaseGenerator *g, double x0, double T, uint64_t steps) {
            return sde->solve(g, x0, T);
        }
    };

    class EulerMaruyamaScheme : public SimulationScheme {
    public:
        double trajectory(SDE *sde, BaseGenerator *g, double x0, double T, uint64_t steps) {
            double dt = T / steps, prev = x0, curr, nPrev = NormalDistribution::rand(g, 0.0, 1.0), nCurr, dW;
            for(uint64_t i = 0; i < steps; ++i) {
                nCurr = NormalDistribution::rand(g, 0.0, 1.0);
                dW = nCurr - nPrev;
                curr = prev + sde->drift(prev, 0.0) * dt + sde->diffusion(prev, 0.0) * dW; // todo: fix placeholder 0.0
                prev = curr;
            }
            return curr;
        }
    };

    // todo:
    class MilsteinScheme : public SimulationScheme {
    public:
        double trajectory(SDE *sde, BaseGenerator *g, double x0, double T, uint64_t steps) {
            return 0.0;
        }
    };

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

    class MonteCarloJob {
        std::function<double(BaseGenerator*)> callback;
    public:
        explicit MonteCarloJob(std::function<double(BaseGenerator*)>& callback) : callback(std::move(callback)) { }
        double compute(BaseGenerator *g) const { return callback(g); };
    };

    class MonteCarloEngine {
    public:
        double run(MonteCarloJob &job, RunSettings &settings) {
            switch(settings.getType()) {
                case RunType::CONCURRENT:
                    return concurrentRun(job, settings.getGenerator(), settings.getPaths());
                case RunType::SEQUENTIAL:
                    return sequentialRun(job, settings.getGenerator(), settings.getPaths());
            }
        }
    private:
        double sequentialRun(MonteCarloJob &job, BaseGenerator *g, uint64_t nPaths) {
            std::vector<double> results;
            double localResult, result = 0.0;
            for(uint64_t i = 0; i < nPaths; ++i) {
                localResult = job.compute(g);
                results.push_back(localResult);
                result += localResult;
            }
            return result / (double) nPaths;
        }
        double concurrentRun(MonteCarloJob &job, BaseGenerator *g, uint64_t nPaths) {
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

#endif //QPP_SIMULATIONUTILS_H
