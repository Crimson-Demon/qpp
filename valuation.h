#ifndef QPP_VALUATION_H
#define QPP_VALUATION_H

#include "marketmodel.h"
#include "option.h"

enum class ValuationType { // or should valuations inherit interfaces??
    ANALYTIC,
    SIMULATION,
    PDE
};

template<typename MarketModel>
class Valuation {
    virtual double valuate(MarketModel* m, Option* o) = 0;
};

template<>
class BSValuation : public Valuation<BSModel> {
    double valuate(BSModel* m, Option* o) {
        return 0;
    }
};


#endif //QPP_VALUATION_H
