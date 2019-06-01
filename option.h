#ifndef QPP_OPTION_H
#define QPP_OPTION_H

#include "security.h"
#include "payoff.h"

namespace qpp {
    class Option : public Security {
        Security *underlying;
        double timeToMaturity;
        double strike;
        Payoff *payoff;

    public:
        Option(Security *underlying, double timeToMaturity, double strike, Payoff *payoff)
                : Security("Option"), underlying(underlying), timeToMaturity(timeToMaturity), strike(strike),
                  payoff(payoff) {}

        ~Option() { delete payoff; }

        OptionRight get_right() const { return payoff->get_right(); }

        OptionStyle get_style() const { return payoff->get_style(); }

        double get_time_to_maturity() const { return timeToMaturity; }

        double get_strike() const { return strike; }

        Security *get_underlying() const { return underlying; }

        Payoff *get_payoff() const { return payoff; }
    };

    class OptionFactory {
    public:
        static Option get_eu_option(OptionRight right, Security *sec, double ttm, double strike) {
            return Option(sec, ttm, strike, new EuPayoff(right, strike));
        }
    };

}

#endif //QPP_OPTION_H
