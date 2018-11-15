#ifndef QPP_VALUATION_H
#define QPP_VALUATION_H

#include <iostream>
#include "marketmodel.h"
#include "option.h"
#include "valuationutils.h"
#include "utils.h"

class ValuationModel {
public:
    virtual double value(MarketModel *m, Option *o, ValuationParameters* v) = 0;
};

template <typename ModelType, typename OptionType>
class GenericValuationModel : public ValuationModel {
public:
    double value(MarketModel *m, Option *o, ValuationParameters* v) final {
        std::cout << "Running base valuation implementation\n";
        auto* cast_m = static_cast<ModelType*>(m);
        auto* cast_o = static_cast<OptionType*>(o);
        return value(cast_m, cast_o, v);
    }
    double value(ModelType* m, OptionType* o, ValuationParameters* v) {
        std::cout << "Running type correct pointer generic valuation\n";
        return value(*m, *o, v);
    }
    double value(ModelType& m, OptionType& o, ValuationParameters* v) {
        std::cout << "Running type correct reference generic valuation\n";
        switch(v->getType()) {
            case ValuationType::ANALYTIC: {
                auto *static_v = dynamic_cast<AnalyticParameters *>(v);
                return ValuationValue<ModelType, OptionType>::compute(m, o, *static_v);
            }
            case ValuationType::SIMULATION: {
                auto *static_v = dynamic_cast<SimulationParameters *>(v);
                return ValuationValue<ModelType, OptionType>::compute(m, o, *static_v);
            }
            case ValuationType::PDE: {
                auto *static_v = dynamic_cast<PDEParameters *>(v);
                return ValuationValue<ModelType, OptionType>::compute(m, o, *static_v);
            }
        }
    }
};

template <typename OptionType>
using BSValuationModel = GenericValuationModel<BSModel, OptionType>;

#endif //QPP_VALUATION_H
