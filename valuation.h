#ifndef QPP_VALUATION_H
#define QPP_VALUATION_H

#include "marketmodel.h"
#include "option.h"

enum class ValuationType { // or should valuations inherit interfaces??
    ANALYTIC,
    SIMULATION,
    PDE
};

class BSValuation;

template<typename ModelType, typename OptionType>
class Valuation {
public:
    static double valuate(MarketModel* m, Option* o, ValuationType valuationType) {
        return BSValuation::valuate(static_cast<ModelType>(m), static_cast<OptionType>(o), valuationType);
    }
private:
    /**
     * Here we list the implementations
     */
    class BSValuation { // do we need the class wrapper here? seems a bit redundant... ah yes, for scope!
    public:
        static double valuate(BSModel* m, Option* o, ValuationType valuationType) {
            switch(valuationType) {
                case ValuationType::ANALYTIC:
                    return BSValuation::cf_valuate(m, o);
                case ValuationType::SIMULATION:
                    return BSValuation::mc_valuate(m, o);
                case ValuationType::PDE:
                    return BSValuation::pde_valuate(m, o);
            }
        }
    private:
        static double cf_valuate(BSModel* m, Option* o) {
            return 1.0;
        }
        static double mc_valuate(BSModel* m, Option* o) {
            return 2.0;
        }
        static double pde_valuate(BSModel* m, Option* o) {
            return 3.0;
        }
    };
};

//inline double valuate(BSModel* m, Option* o, ValuationType valuationType) {
//    return 2.0;
//}

#endif //QPP_VALUATION_H
