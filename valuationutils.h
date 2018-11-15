#ifndef QPP_VALUATIONUTILS_H
#define QPP_VALUATIONUTILS_H


#include "marketmodel.h"
#include "option.h"

class MonteCarloEngine;

class Grid;

class ComputationScheme {};
class SimulationScheme : public ComputationScheme {};
class EulerMaruyamaScheme : public SimulationScheme {};
class MilsteinScheme : public SimulationScheme {};
class NumericScheme : public ComputationScheme {};

enum class ValuationType { // or should valuations inherit interfaces??
    ANALYTIC,
    SIMULATION,
    PDE
};

class ValuationParameters {
public:
    virtual ValuationType getType() = 0;
};

class AnalyticParameters : public ValuationParameters {
public:
    ValuationType getType() final {
       return ValuationType::ANALYTIC;
    }
};

class SimulationParameters : public ValuationParameters {
    uint64_t n_paths;
    MonteCarloEngine* engine;
    SimulationScheme* scheme;
public:
    ValuationType getType() final {
        return ValuationType::SIMULATION;
    }
};

class PDEParameters : public ValuationParameters {
    NumericScheme* scheme;
    Grid* grid;
public:
    ValuationType getType() final {
        return ValuationType::PDE;
    }
};

template <typename ModelType, typename OptionType>
class ValuationValue;

template<>
class ValuationValue<BSModel, EuropeanOption> {
public:
    static double compute(BSModel& m, EuropeanOption& o, AnalyticParameters& v) {
        return 0.0;
    }
    static double compute(BSModel& m, EuropeanOption& o, SimulationParameters& v) {
        return 1.0;
    }
    static double compute(BSModel& m, EuropeanOption& o, PDEParameters& v) {
        return 2.0;
    }
};

#endif //QPP_VALUATIONUTILS_H
