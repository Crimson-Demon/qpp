#ifndef NUMLIB_FDM_H
#define NUMLIB_FDM_H


#include <functional>
#include <cmath>
#include <iostream>

#include "pde.h"

namespace fdm {
    class FDM {
    public:
        virtual ~FDM() = default;

        virtual dvec
        step(const pde::BoundaryValueProblem &problem, const dvec &u0, double t0, const dvec &x0, double dt,
             double dx, bool verbose) const = 0;
    };

    class ExplicitScheme : public FDM {
    public:
        ExplicitScheme() = default;

        ~ExplicitScheme() = default;

        dvec
        step(const pde::BoundaryValueProblem &problem, const dvec &u0, double t0, const dvec &x0, double dt,
             double dx, bool verbose) const final {
            dvec u(u0.size());
            u[0] = problem.left_limit(t0);
            if (verbose)
                std::cout << "u0: " << u[0] << std::endl;
            for (unsigned i = 1; i < x0.size() - 1; ++i) {
                double a = dt * problem.get_pde().diffusion(t0, x0[i]) / (dx * dx);
                double b = dt * problem.get_pde().drift(t0, x0[i]) / dx;
                double coeff1 = (1 - 2 * a);
                double coeff2 = a + b;
                double coeff3 = a - b;
                double sourceTerm = dt * problem.get_pde().source(t0, u0[i]);

                u[i] = coeff1 * u0[i] + coeff2 * u0[i + 1] + coeff3 * u0[i - 1] + sourceTerm;
                if (verbose)
                    std::cout << "u" << i << ": " << u[i] << std::endl;
            }
            u[x0.size() - 1] = problem.right_limit(t0);
            if (verbose)
                std::cout << "u" << x0.size() - 1 << ": " << u[x0.size() - 1] << std::endl;
            return u;
        }
    };

    class ImplicitScheme : public FDM {
    public:
        ImplicitScheme() = default;

        ~ImplicitScheme() = default;

        dvec
        step(const pde::BoundaryValueProblem &problem, const dvec &u0, double t0, const dvec &x0, double dt,
             double dx, bool verbose) const final {
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
             double dx, bool verbose) const final {
            dvec u;
            return u;
        }
    };
};


#endif //NUMLIB_FDM_H
