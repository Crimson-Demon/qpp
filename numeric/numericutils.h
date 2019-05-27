#ifndef NUMLIB_NUMERICUTILS_H
#define NUMLIB_NUMERICUTILS_H

#include <vector>
#include <cmath>
#include <functional>

namespace num {
    class Grid;

//    class ComputationScheme {
//    };

    class Differential {
        unsigned order;
        std::vector<unsigned> variables;
    };

    template<int order>
    using PDEComponent = std::tuple<double, std::function<double(double[order])>, Differential>;

    // drift diffusion pde
    template<int order>
    class PDE {
        std::vector<PDEComponent<order>> def;
    public:
        PDE(std::vector<PDEComponent<order>> def) : def(std::move(def)) {}
    };

    template<int order>
    class PDEFactory {
    public:
        static PDE<order> makeHeatPDE() {

        }
        static PDE<order> makeBSPDE() {

        }
    };

    class BoundaryCondition {

    };

    enum class DifferenceType {
        FORWARD, BACKWARD, CENTRAL
    };

    class Difference {
        unsigned order;
        double increment;
        DifferenceType type;
    public:
        Difference(unsigned order, double increment, DifferenceType type) : order(order), increment(increment), type(type) {}
        unsigned getOrder() const { return order; }
        double getIncrement() const { return increment; }
        DifferenceType getType() const { return type; }
    };

    class Scheme {
        std::vector<Difference> def;
    public:
        Scheme() = default;
        Scheme(std::vector<Difference> def) : def(std::move(def)) {}
        void addDifference(Difference diff) { def.push_back(diff); }
    };

    class SchemeBuilder {
        Scheme scheme;
    public:
        SchemeBuilder() = default;
        SchemeBuilder& start() { Scheme newScheme; scheme = newScheme; return *this; }
        SchemeBuilder& add(Difference diff) { scheme.addDifference(diff); return *this; }
        SchemeBuilder& addDifference(unsigned order, double increment, DifferenceType type) {
            Difference diff(order, increment, type);
            scheme.addDifference(diff);
            return *this;
        }
        Scheme finish() { return scheme; }
    };

    /*class NumericScheme :public ComputationScheme {
    public:
        virtual std::vector<double> step(PDE *, std::vector<double>, double, double) = 0;
    };
    */

    class FDM {
    public:
        virtual std::vector<double> step(PDE *, std::vector<double>, double , double, double, double) = 0;
    };

    class ExplicitScheme : public FDM {
    public:
        std::vector<double> step(PDE *pde, std::vector<double> x0, double xbl, double xbr, double dx, double dt) {
            std::vector<double> x1 = x0;
            unsigned size = x0.size();
            double r = dt / std::pow(dx, 2);
            x1[0] = xbl;
            x1[size - 1] = xbr;
            for (unsigned i = 1; i < size - 1; ++i) {
                x1[i] = (1 - 2 * r) * x0[i] + r * (x0[i - 1] + x0[i + 1]);
            };
            return x1;
        }
    };

    class PDESolver {
    public:
        std::vector<double> solve(PDE *pde, FDM *scheme, const std::vector<double> &x0,
                                  double dx, unsigned steps, double dt) {
            auto x = x0;
            for (unsigned i = 0; i < steps; ++i) {
                x = scheme->step(pde, x, dx, dt);
            }
            return x;
        }
    };
}

#endif //NUMLIB_NUMERICUTILS_H
