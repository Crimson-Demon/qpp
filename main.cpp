#include <iostream>
//#include "marketmodel.h"
//#include "option.h"
//#include "security.h"
//#include "valuation.h"
#include "qpp.h"

int main() {

    Security* s = new Security("Sec1");
//    Security* s2 = new Security("Sec2");
    MarketModel* m = new BSModel(0.05, 0.2);
    m->addSecurity(s, 100);
    Option* o = new EuropeanOption(OptionRight::CALL, s, 1, 100);
    ValuationParameters* vp = new AnalyticParameters();
    ValuationModel* v = new BSValuationModel<EuropeanOption>();
    std::cout << v->value(m, o, vp) << std::endl;
//    std::cout << NormalDistribution::cdf(0, 0.0, 1.0) << std::endl;
    return 0;
}