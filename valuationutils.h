#ifndef QPP_VALUATIONUTILS_H
#define QPP_VALUATIONUTILS_H


#include <functional>
#include <cmath>
#include <future>
#include <random>

#include "marketmodel.h"
#include "option.h"
#include "simulationutils.h"
#include "numericutils.h"

namespace qpp {
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
        MonteCarloEngine *engine;
        SimulationScheme *scheme;
        uint64_t simulationSteps;
        RunSettings *settings;
    public:
        ValuationType getType() const final { return ValuationType::SIMULATION; }
        MonteCarloEngine* getEngine() const { return engine; }
        SimulationScheme* getScheme() const { return scheme; }
        uint64_t getSteps() const { return simulationSteps; }
        RunSettings* getSettings() const { return settings; }
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
            return optionSign * NormalDistribution::std_cdf(optionSign * d1) * m.getSpot(o.getUnderlying()) + (-optionSign) * NormalDistribution::std_cdf(optionSign * d2) * o.getStrike() * std::exp(-m.getRate() * o.getTimeToMaturity());
        }

        static double compute(BSModel &m, EuropeanOption &o, SimulationParameters &p) {
            MonteCarloEngine* engine = p.getEngine();
            SimulationScheme* scheme = p.getScheme();
            RunSettings* settings = p.getSettings();
            std::function<double(BaseGenerator*)> callback = [&m, &o, scheme](BaseGenerator *g)->double{ return std::exp(-m.getRate() * o.getTimeToMaturity()) * o.payoff(scheme->trajectory(m.getSDE(), g, m.getSpot(o.getUnderlying()), o.getTimeToMaturity(), p.getSteps())); };
            MonteCarloJob* job = new MonteCarloJob(callback);
            return engine->run(*job, *settings);
        }

        static double compute(BSModel &m, EuropeanOption &o, PDEParameters &p) {
            return 2.0;
        }
    };

}

#endif //QPP_VALUATIONUTILS_H
