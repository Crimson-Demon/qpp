#ifndef QPP_MARKETMODEL_H
#define QPP_MARKETMODEL_H

#include <vector>

#include "security.h"

class MarketModel {
public:
    virtual ~MarketModel() = 0;
};

//inline MarketModel::~MarketModel() {}

class BSModel : public MarketModel {
    double rate;
    double variance;
    std::vector<Security*> assets;
};

#endif //QPP_MARKETMODEL_H
