#ifndef NUMLIB_PDE_H
#define NUMLIB_PDE_H

#include <functional>
#include <vector>
#include <cmath>

using dvec = std::vector<double>;

namespace pde {

    // drift-diffusion equation
    class PDE {
        std::function<double(double, double)> driftCoefficient; // first-order diff operator
        std::function<double(double, double)> diffusionCoefficient; // second-order diff operator
        std::function<double(double, double)> sourceCoefficient;
    public:
        PDE(std::function<double(double, double)> driftCoefficient,
            std::function<double(double, double)> diffusionCoefficient,
            std::function<double(double, double)> sourceCoefficient)
                : driftCoefficient(std::move(driftCoefficient)), diffusionCoefficient(std::move(diffusionCoefficient)),
                  sourceCoefficient(std::move(sourceCoefficient)) {}

        double drift(double t, double x) const { return driftCoefficient(t, x); }

        double diffusion(double t, double x) const { return diffusionCoefficient(t, x); }

        double source(double t, double x) const { return sourceCoefficient(t, x); }
    };

    class BoundaryValueProblem {
        PDE pde;
        std::function<double(double)> initialValue;
        std::function<double(double)> leftBoundary;
        std::function<double(double)> rightBoundary;
    public:
        BoundaryValueProblem(PDE pde, std::function<double(double)> initialValue,
                             std::function<double(double)> leftBoundary, std::function<double(double)> rightBoundary)
                : pde(std::move(pde)), initialValue(std::move(initialValue)), leftBoundary(std::move(leftBoundary)),
                  rightBoundary(std::move(rightBoundary)) {}

        const PDE &get_pde() const { return pde; }

        double initialize(double d) const { return initialValue(d); }

        double left_limit(double t) const { return leftBoundary(t); }

        double right_limit(double t) const { return rightBoundary(t); }
    };

    class PDEFactory {
    public:
        static BoundaryValueProblem makeHeatPDE(const std::function<double(double, double)> &diffusion,
                                                const std::function<double(double)> &initialValue,
                                                const std::function<double(double)> &left,
                                                const std::function<double(double)> &right) {
            std::function<double(double, double)> zero = [](double t, double x) -> double { return 0.0; };
            PDE pde(zero, diffusion, zero);
            BoundaryValueProblem problem(pde, initialValue, left, right);
            return problem;
        }

        static BoundaryValueProblem makeBSPDE(double r, double v, const std::function<double(double)> &payoff,
                                              const std::function<double(double)> &left,
                                              const std::function<double(double)> &right) {
            std::function<double(double, double)> drift = [r](double t, double s) -> double { return r * s; };
            std::function<double(double, double)> diffusion = [v](double t, double s) -> double {
                return 0.5 * std::pow(v * s, 2);
            };
            std::function<double(double, double)> source = [r](double t, double v) -> double { return -r * v; };
            PDE pde(drift, diffusion, source);
            BoundaryValueProblem problem(pde, payoff, left, right);
            return problem;
        }
    };
};

#endif //NUMLIB_PDE_H
