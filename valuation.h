#ifndef QPP_VALUATION_H
#define QPP_VALUATION_H

#include <iostream>
#include "marketmodel.h"
#include "option.h"
#include "valuationutils.h"
#include "utils.h"

namespace qpp {

    class ValuationModel {
    public:
        virtual double value(MarketModel *m, Option *o, ValuationParameters *vp) = 0;
    };

    template<typename ModelType, typename OptionType>
    class GenericValuationModel : public ValuationModel {
    public:
        double value(MarketModel *m, Option *o, ValuationParameters *vp) final {
            std::cout << "Running base valuation implementation\n";
//            try {
//                m->getSpot(o->getUnderlying());
//            } catch (std::exception &e) {
//                std::cerr << "Model doesn't contain underlying information." << std::endl;
//                std::cerr << e.what() << std::endl;
//            }
            auto *cast_m = static_cast<ModelType *>(m);
            auto *cast_o = static_cast<OptionType *>(o);
            return value(cast_m, cast_o, vp);
        }

        double value(ModelType *m, OptionType *o, ValuationParameters *vp) {
            std::cout << "Running type correct pointer generic valuation\n";
            return value(*m, *o, vp);
        }

        double value(ModelType &m, OptionType &o, ValuationParameters *vp) {
            std::cout << "Running type correct reference generic valuation\n";
            double result;
            switch (vp->getType()) {
                case ValuationType::ANALYTIC: {
                    auto *cast_vp = dynamic_cast<AnalyticParameters *>(vp);
                    result =  ValuationValue<ModelType, OptionType>::compute(m, o, *cast_vp);
                    break;
                }
                case ValuationType::SIMULATION: {
                    auto *static_v = dynamic_cast<SimulationParameters *>(vp);
                    result = ValuationValue<ModelType, OptionType>::compute(m, o, *static_v);
                    break;
                }
                case ValuationType::PDE: {
                    auto *static_v = dynamic_cast<PDEParameters *>(vp);
                    result = ValuationValue<ModelType, OptionType>::compute(m, o, *static_v);
                    break;
                }
            }
            return result;
        }
    };

    template<typename OptionType>
    using BSValuationModel = GenericValuationModel<BSModel, OptionType>;

}

#endif //QPP_VALUATION_H
