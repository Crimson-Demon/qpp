#ifndef QPP_MARKETMODEL_H
#define QPP_MARKETMODEL_H

#include <vector>
#include <map>

#include "security.h"
#include "simulationutils.h"

namespace qpp {

    // todo: the sde isnt very practically placed here
    // todo: need a better interface for it
    class MarketModel {
        std::map<Security *, double> spotMap;
        SDE* sde;
    public:
        MarketModel(SDE* sde) : sde(sde) { }
        void addSecurity(Security *security, double spot) {
            spotMap.insert(std::make_pair(security, spot));
        }
        bool hasSecurity(Security *security) const {
            return spotMap.find(security) != spotMap.end();
        }
        double getSpot(Security *security) const {
            return spotMap.at(security);
        }
        void removeSecurity(Security *security) { spotMap.erase(security); }
        SDE* getSDE() const { return sde; }
        virtual ~MarketModel() = 0;
    };

    inline MarketModel::~MarketModel() = default;

    class BSModel : public MarketModel {
        double rate;
        double volatility;
    public:
        BSModel(double rate, double volatility) : MarketModel(new GBM(rate, volatility)), rate(rate), volatility(volatility) { };
        double getRate() const { return rate; }
        double getVolatility() const { return volatility; }
    };
}

#endif //QPP_MARKETMODEL_H
