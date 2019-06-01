#include <iostream>
#include "qpp.h"

int main() {

    Security *s = new Security("Sec1");
    BSModel m(0.05, 0.2);
    m.add_security(s, 100);
    double ttm = 1, strike = 100;
    Option o = OptionFactory::get_eu_option(OptionRight::CALL, s, ttm, strike);
    CFParameters cfp;
    MCParameters mcp;
    PDEParameters pdep;
    std::cout << value(m, o, cfp) << std::endl;
    std::cout << value(m, o, mcp) << std::endl;
    std::cout << value(m, o, pdep) << std::endl;
    delete s;
    return 0;
}