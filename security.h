#ifndef QPP_SECURITY_H
#define QPP_SECURITY_H

#include <string>

namespace qpp {

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

        explicit Security(std::string name) : name(std::move(name)) {};
    };

}

#endif //QPP_SECURITY_H
