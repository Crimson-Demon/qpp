#include <iostream>
#include "marketmodel.h"
#include "option.h"
#include "valuation.h"

int main() {
    auto* s = new Security();
    MarketModel* m = new BSModel(s, 0.05, 0.2);
    Option* o = new EuropeanOption(OptionRight::CALL, s, 1, 100);
    Valuation* v = new BSValuation<EuropeanOption, ValuationType::ANALYTIC>();
    std::cout << v->value(m, o) << std::endl;
    return 0;
}