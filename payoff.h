#ifndef QPP_PAYOFF_H
#define QPP_PAYOFF_H

#include <functional>
#include <map>
#include <algorithm>

enum class OptionStyle {
    EUROPEAN,
    AMERICAN,
    BERMUDAN,
    ASIAN,
    EXOTIC
};

enum class OptionRight {
    CALL,
    PUT
};

class Payoff {
    OptionStyle style;
    OptionRight right;

public:
    Payoff(OptionRight right, OptionStyle style) : style(style), right(right) {}

    virtual ~Payoff() = default;

    OptionStyle get_style() const { return style; }

    OptionRight get_right() const { return right; }

    virtual double operator()(double val) = 0;
};

class EuPayoff : public Payoff {
    double strike;
public:
    EuPayoff(OptionRight right, double strike) : Payoff(right, OptionStyle::EUROPEAN), strike(strike) {}

    double operator()(double val) {
        switch (get_right()) {
            case OptionRight::CALL:
                return std::max<double>(val - strike, 0);
            case OptionRight::PUT:
                return std::max<double>(strike - val, 0);
            default:
                return 0.0;
        }
    }
};


#endif //QPP_PAYOFF_H
