#ifndef QPP_OPTION_H
#define QPP_OPTION_H

#include "security.h"
//#include "payoff.h"


enum class OptionRight {
    CALL,
    PUT
};

enum class OptionStyle {
    EUROPEAN,
    AMERICAN,
    BERMUDAN,
    ASIAN,
    EXOTIC
};

// IDEA? Maybe I should template the option with the payoff (and keep the boons of compostion, decouple the class a bit and use templated option classes in the signature of valuation implementations?
class Option : public Security {
    OptionRight right;
    OptionStyle style;
    double timeToMaturity;
    double strike;
    Security* underlying; // should this be here or in the model?

public:
    Option(OptionRight optionRight, Security* underlying, double timeToMaturity, double strike, OptionStyle style) : Security("Option"), right(optionRight), underlying(underlying), timeToMaturity(timeToMaturity), strike(strike), style(style) { }
    OptionRight getRight() const { return right; }
    OptionStyle getStyle() const { return style; }
    double getTimeToMaturity() const { return timeToMaturity; }
    double getStrike() const { return strike; }
    Security* getUnderlying() const { return underlying; }
    virtual double payoff(double value) = 0;
};

class EuropeanOption : public Option {
public:
    EuropeanOption(OptionRight optionRight, Security* underlying, double timeToMaturity, double strike) : Option(optionRight, underlying, timeToMaturity, strike, OptionStyle::EUROPEAN) { }
    double payoff(double value) {
        switch(getRight()) {
            case OptionRight::CALL:
                return std::max<double>(value - getStrike(), 0);
            case OptionRight::PUT:
                return std::max<double>(getStrike() - value, 0);
        }
    }
};

/*
 * Moze bedziemy trzymac shared_ptr payoffow tu, by nie zbednie tworzyc nowych przy budowaniu opcji?
 */

/*
class OptionFactory {
};

class OptionBuilder {
};
*/

#endif //QPP_OPTION_H
