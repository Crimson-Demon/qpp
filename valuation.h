#ifndef QPP_VALUATION_H
#define QPP_VALUATION_H

#include <iostream>
#include "marketmodel.h"
#include "option.h"

enum class ValuationType { // or should valuations inherit interfaces??
    ANALYTIC,
    SIMULATION,
    PDE
};

template <typename ModelType, typename OptionType, ValuationType vt>
class ValuationValue;

template<>
class ValuationValue<BSModel, EuropeanOption, ValuationType::ANALYTIC> {
public:
    static double compute(BSModel& m, EuropeanOption& o) {
        return 0.0;
    }
};

template<>
class ValuationValue<BSModel, EuropeanOption, ValuationType::SIMULATION> {
public:
    static double compute(BSModel& m, EuropeanOption& o) {
        return 1.0;
    }
};

template<>
class ValuationValue<BSModel, EuropeanOption, ValuationType::PDE> {
    static double compute(BSModel& m, EuropeanOption& o) {
        return 2.0;
    }
};

class Valuation {
public:
    virtual double value(MarketModel *m, Option *o) = 0;
};

template <typename ModelType, typename OptionType, ValuationType vt>
class GenericValuation : public Valuation {
public:
    double value(MarketModel *m, Option *o) {
        std::cout << "Running base valuation implementation \n";
        auto* cast_m = static_cast<ModelType*>(m);
        auto* cast_o = static_cast<OptionType*>(o);
        return value(cast_m, cast_o);
    }
    double value(ModelType* m, OptionType* o) {
        std::cout << "Running type correct pointer generic valuation\n";
        return value(*m, *o);
    }
    double value(ModelType& m, OptionType& o) {
        std::cout << "Running type correct reference generic valuation\n";
        return ValuationValue<ModelType, OptionType, vt>::compute(m, o);
    }
};

template <typename OptionType, ValuationType vt>
using BSValuation = GenericValuation<BSModel, OptionType, vt>;

#endif //QPP_VALUATION_H
