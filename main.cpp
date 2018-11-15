#include <iostream>
#include "marketmodel.h"
#include "option.h"
#include "valuation.h"

int main() {
    auto* s = new Security();
    MarketModel* m = new BSModel(s, 0.05, 0.2);
    Option* o = new EuropeanOption(OptionRight::CALL, s, 1, 100);
    ValuationParameters* vp = new AnalyticParameters();
    ValuationModel* v = new BSValuationModel<EuropeanOption>();
    std::cout << v->value(m, o, vp) << std::endl;
    return 0;
}