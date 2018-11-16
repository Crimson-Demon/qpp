#include <iostream>
//#include "marketmodel.h"
//#include "option.h"
//#include "security.h"
//#include "valuation.h"
#include "qpp.h"

int main() {

    auto* s = new qpp::Security();
    MarketModel* m = new BSModel(0.05, 0.2);
    m->insert(s, 100);
    Option* o = new EuropeanOption(OptionRight::CALL, s, 1, 100);
    ValuationParameters* vp = new AnalyticParameters();
    ValuationModel* v = new BSValuationModel<EuropeanOption>();
    std::cout << v->value(m, o, vp) << std::endl;
    return 0;
}