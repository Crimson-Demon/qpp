#ifndef QPP_MARKETMODEL_H
#define QPP_MARKETMODEL_H

#include <vector>
#include <map>

#include "security.h"
#include "simulation/sde.h"
#include "valuationutils.h"
#include "option.h"
#include "simulation/random.h"

namespace qpp {

    class MarketModel {
        std::map<Security *, double> spotMap;
    public:
        MarketModel() = default;

        void add_security(Security *security, double spot) {
            spotMap.insert(std::make_pair(security, spot));
        }

        bool has_security(Security *security) const {
            return spotMap.find(security) != spotMap.end();
        }

        double get_spot(Security *security) const {
            return spotMap.at(security);
        }

        void remove_security(Security *security) { spotMap.erase(security); }

        virtual ~MarketModel() = 0;
    };

    inline MarketModel::~MarketModel() = default;

    class BSModel : public MarketModel {
        double rate;
        double volatility;
    public:
        BSModel(double rate, double volatility) : MarketModel(), rate(rate), volatility(volatility) {};

        double get_rate() const { return rate; }

        double get_vol() const { return volatility; }
    };
}

#endif //QPP_MARKETMODEL_H
