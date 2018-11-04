#ifndef QPP_OPTION_H
#define QPP_OPTION_H

#include "security.h"
#include "payoff.h"


/*enum class OptionRight {
    CALL,
    PUT
};
*/

class Option : public Security {
//    OptionRight right;
    Payoff* payoff;
    double timeToMaturity;
    Security* underlying;

public:
    Option(Security* underlying, Payoff* payoff, double timeToMaturity) : // niech to bedzie friend z optionfactory
            underlying(underlying), payoff(payoff), timeToMaturity(timeToMaturity) { }
    OptionRight getRight() const { return payoff->getRight(); }
    OptionStyle getStyle() const { return payoff->getStyle(); }
    ~Option() { delete payoff; }
};

/*
 * Moze bedziemy trzymac shared_ptr payoffow tu, by nie zbednie tworzyc nowych
 */

class OptionFactory {
public:
    static Option* makeEuropeanOption(Security* underlying, double strike, double timeToMaturity, OptionRight right) {
        Payoff* p = new EuPayoff(right);
        Option* o = new Option(underlying, p, timeToMaturity);
        return o;
    }
};
/*
class OptionBuilder {
};
*/

#endif //QPP_OPTION_H
