#ifndef QPP_VALUATIONUTILS_H
#define QPP_VALUATIONUTILS_H


#include <functional>
#include <cmath>

#include "marketmodel.h"
#include "option.h"

namespace qpp {

    // todo: cannot use double/float as template classes, look for reasonable workaround to template with double
    class NormalDistribution {
    public:
        //todo: check if correct formula connecting normal cdf and error function
        static constexpr double std_cdf(double value) { return 0.5 * (1 + erfc((value) / (M_SQRT1_2))); }
        static constexpr double cdf(double value, double mean, double variance) { return 0.5 * (1 + erfc((value) / (M_SQRT1_2))); }
    };

    class Grid;

    class ComputationScheme {
    };

    class SimulationScheme : public ComputationScheme {
    };

    class EulerMaruyamaScheme : public SimulationScheme {
    };

    class MilsteinScheme : public SimulationScheme {
    };

    class NumericScheme : public ComputationScheme {
    };

    class ConcurrencySettings {

    };

    class MonteCarloJob {
        std::function<double(double)> callback;
    public:
        MonteCarloJob(std::function<double(double)>& callback) : callback(std::move(callback)) { }
    };

    class MonteCarloEngine {
    public:
        double run(MonteCarloJob& job, uint64_t nPaths, SimulationScheme& scheme, ConcurrencySettings& settings) {
            return 5.0;
        }
    };

    enum class ValuationType { // or should valuations inherit interfaces??
        ANALYTIC,
        SIMULATION,
        PDE
    };

    class ValuationParameters {
    public:
        virtual ValuationType getType() const = 0;
    };

    class AnalyticParameters : public ValuationParameters {
    public:
        ValuationType getType() const final { return ValuationType::ANALYTIC; }
    };

    class SimulationParameters : public ValuationParameters {
        uint64_t n_paths;
        MonteCarloEngine *engine;
        SimulationScheme *scheme;
        ConcurrencySettings* settings;
    public:
        ValuationType getType() const final { return ValuationType::SIMULATION; }
        uint64_t getPaths() const { return n_paths; }
        MonteCarloEngine* getEngine() const { return engine; }
        SimulationScheme* getScheme() const { return scheme; }
        ConcurrencySettings* concurrency() const { return settings; };
    };

    class PDEParameters : public ValuationParameters {
        NumericScheme *scheme;
        Grid *grid;
    public:
        ValuationType getType() const final { return ValuationType::PDE; }
    };

    template<typename ModelType, typename OptionType>
    class ValuationValue;

    template<>
    class ValuationValue<BSModel, EuropeanOption> {
    public:
        static double compute(BSModel &m, EuropeanOption &o, AnalyticParameters &p) {
            double d1 = (std::log(m.getSpot(o.getUnderlying()) / o.getStrike()) + (m.getRate() + 0.5 * std::pow(m.getVolatility(), 2)) * o.getTimeToMaturity()) / (m.getVolatility() * std::sqrt(o.getTimeToMaturity()));
            double d2 = d1 - m.getVolatility() * std::sqrt(o.getTimeToMaturity());
            double optionSign = o.getRight() == OptionRight::CALL ? 1.00 : -1.00;
            return NormalDistribution::std_cdf(optionSign * d1) * m.getSpot(o.getUnderlying()) + optionSign * NormalDistribution::std_cdf(optionSign * d2) * o.getStrike() * std::exp(-m.getRate() * o.getTimeToMaturity());
        }

        static double compute(BSModel &m, EuropeanOption &o, SimulationParameters &p) {
            uint64_t nPaths = p.getPaths();
            MonteCarloEngine* engine = p.getEngine();
            SimulationScheme* scheme = p.getScheme();
            ConcurrencySettings* settings = p.concurrency();
            std::function<double(double)> callback = [&m, &o](double sT)->double{ return std::exp(-m.getRate() * o.getTimeToMaturity()) * o.payoff(sT); };
            MonteCarloJob* job = new MonteCarloJob(callback);
            return engine->run(*job, nPaths, *scheme, *settings);
        }

        static double compute(BSModel &m, EuropeanOption &o, PDEParameters &p) {
            return 2.0;
        }
    };

}

#endif //QPP_VALUATIONUTILS_H
