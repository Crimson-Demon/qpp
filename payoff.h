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
//    std::function<double(std::unordered_map<string, double>)> func;
//    std::unordered_map<std::string, double> params;
    OptionStyle style;
    OptionRight right;

public:
    Payoff(OptionRight right, OptionStyle style) : right(right), style(style) { }
    OptionStyle getStyle() const { return style; }
    OptionRight getRight() const { return right; }
    virtual double operator()() = 0;

//    virtual ~Payoff() = 0;
};

class EuPayoff : public Payoff {
    double strike;
public:
    EuPayoff(OptionRight right) : Payoff(right, OptionStyle::EUROPEAN) { }
    double operator()(double underlying_value) {
        switch(getRight()) {
            case OptionRight::CALL:
                return std::max<double>(underlying_value - strike, 0);
            case OptionRight::PUT:
                return std::max<double>(strike - underlying_value, 0);
        }
    }
};


#endif //QPP_PAYOFF_H
