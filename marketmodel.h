#ifndef QPP_MARKETMODEL_H
#define QPP_MARKETMODEL_H

#include <vector>
#include <map>

#include "security.h"

namespace qpp {

    class MarketModel {
        std::map<Security *, double> map;
    public:
        void insert(Security *security, double spot) {
            map.insert(std::make_pair(security, spot));
        }
        double getSpot(Security *security) { return map[security]; }
        void erase(Security *security) { map.erase(security); }
        virtual ~MarketModel() = 0;
    };

    inline MarketModel::~MarketModel() = default;

    class BSModel : public MarketModel {
        double rate;
        double volatility;
    public:
        BSModel(double rate, double volatility) : rate(rate), volatility(volatility) { };
        double getRate() const { return rate; }
        double getVolatility() const { return volatility; }
    };
}

#endif //QPP_MARKETMODEL_H
