#ifndef QPP_SECURITY_H
#define QPP_SECURITY_H

#include <string>

/*
 * Czy to potrzebne?
enum class SecurityType {
    EQUITY,
    BOND,
    INTEREST_RATE,
    INDEX,
    COMMODITY,
    CURRENCY
    OPTION,
    FUTURE
};
*/

class Security {
public:
    std::string name;
    Security() : name("Security") { };
    Security(std::string name) : name(std::move(name)) { };
};


#endif //QPP_SECURITY_H
