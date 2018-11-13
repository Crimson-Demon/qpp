#ifndef QPP_VALULATIONVALUE_H
#define QPP_VALULATIONVALUE_H

#include "utils.h"

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


#endif //QPP_VALULATIONVALUE_H
