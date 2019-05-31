#ifndef NUMLIB_FDM_H
#define NUMLIB_FDM_H


#include <functional>
#include <cmath>

#include "pde.h"

namespace fdm {
    class FDM {
    public:
        virtual ~FDM() = default;

        virtual dvec
        step(const pde::BoundaryValueProblem &problem, const dvec &u0, double t0, const dvec &x0, double dt,
             double dx) const = 0;
    };

    class ExplicitScheme : public FDM {
    public:
        ExplicitScheme() = default;

        ~ExplicitScheme() = default;

        dvec
        step(const pde::BoundaryValueProblem &problem, const dvec &u0, double t0, const dvec &x0, double dt,
             double dx) const final {
            dvec u(u0.size());
            u[0] = problem.left_limit(t0);
            for (unsigned i = 1; i < x0.size() - 1; ++i) {
                double a = (1 - 2 * dt * problem.get_pde().diffusion(t0, x0[i]) / std::pow(dx, 2));
                double b = (dt * problem.get_pde().diffusion(t0, x0[i - 1]) / std::pow(dx, 2) +
                            0.5 * dt * problem.get_pde().drift(t0, x0[i - 1]) / dx);
                double c = (dt * problem.get_pde().diffusion(t0, x0[i + 1]) / std::pow(dx, 2) -
                            0.5 * dt * problem.get_pde().drift(t0, x0[i + 1]) / dx);
                double d = dt * problem.get_pde().source(t0, x0[i]);

                u[i] = a * u0[i] + b * u[i - 1] + c * u[i + 1] + d;
            }
            u[x0.size() - 1] = problem.right_limit(t0);
            return u;
        }
    };

    class ImplicitScheme : public FDM {
    public:
        ImplicitScheme() = default;

        ~ImplicitScheme() = default;

        dvec
        step(const pde::BoundaryValueProblem &problem, const dvec &u0, double t0, const dvec &x0, double dt,
             double dx) const final {
            dvec u;
            return u;
        }
    };

    class CrankNicholsonScheme : public FDM {
    public:
        CrankNicholsonScheme() = default;

        ~CrankNicholsonScheme() = default;

        dvec
        step(const pde::BoundaryValueProblem &problem, const dvec &u0, double t0, const dvec &x0, double dt,
             double dx) const final {
            dvec u;
            return u;
        }
    };
};


#endif //NUMLIB_FDM_H
