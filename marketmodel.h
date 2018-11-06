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
    Security* asset; // Moze dajmy aktywo do wyceny tutaj zamiast do opcji? I model bedzie okreslal, co jest wycenialne?
public:
    BSModel(Security* security, double rate, double variance) : asset(security), rate(rate), variance(variance) { }
};

#endif //QPP_MARKETMODEL_H
