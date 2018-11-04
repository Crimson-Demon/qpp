#ifndef QPP_VALUATION_H
#define QPP_VALUATION_H

#include "marketmodel.h"
#include "option.h"

enum class ValuationType { // or should valuations inherit interfaces??
    ANALYTIC,
    SIMULATION,
    PDE
};

/*
 * VARIADIC MAGIC
 */

template<typename MarketModel, typename ... ValuationTypes>
class Valuation {
    template<typename ... ValuationTypes>
    virtual double valuate(MarketModel* m, Option* o) = 0;
};

template<typename MarketModel, typename ValuationType, typename ... ValuationTypes>
class Valuation {
    template<typename ValuationType>
    virtual double valuate(MarketModel* m, Option* o) = 0;
};

/*
 * IMPLEMENT MODELS BELOW HERE FOR NOW
 */

template<>
class BSValuation : public Valuation<BSModel, ValuationType::ANALYTIC, ValuationType::SIMULATION, ValuationType::PDE> {
    template<>
    double valuate<ValuationType::ANALYTIC>(BSModel* m, Option* o) {
        return 0;
    }
    template<>
    double valuate<ValuationType::SIMULATION>(BSModel* m, Option* o) {
        return 0;
    }
    template<>
    double valuate<ValuationType::PDE>(BSModel* m, Option* o) {
        return 0;
    }
};


#endif //QPP_VALUATION_H
