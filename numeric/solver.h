#ifndef NUMLIB_SOLVER_H
#define NUMLIB_SOLVER_H

#include <vector>
#include <cmath>
#include "pde.h"
#include "fdm.h"

namespace num {
    class PDESolver {
    public:
        static double
        solve(const pde::BoundaryValueProblem &problem, const fdm::FDM *scheme, double tmin, double tmax, unsigned nt,
              double xmin, double x0, double xmax, unsigned nx, bool verbose) {
            dvec x = linspace(xmin, xmax, nx, verbose);
            dvec u(x.size());
            if (verbose)
                std::cout << "Initialising u" << std::endl;
            for (unsigned i = 0; i <= nx; ++i) {
                u[i] = problem.initialize(x[i]);
                if (verbose)
                    std::cout << "u" << i << ": " << u[i] << std::endl;
            }
            double t = tmin, dt = (tmax - tmin) / nt, dx = (xmax - xmin) / nx, l = dt / (dx * dx);
            std::cout << "l = " << l << std::endl;
            if (verbose)
                std::cout << "dt = " << dt << ", dx = " << dx << ", l = " << l << std::endl;
            for (unsigned i = 0; i < nt; ++i) {
                if (verbose)
                    std::cout << "Current step is " << i << " of " << nt - 1 << std::endl;
                u = scheme->step(problem, u, t, x, std::abs(dt), dx, verbose);
                t += dt;
            }
            unsigned i = 0;
            for (; i < nx; ++i) {
                if (x[i] > x0)
                    break;
            }
            double z = (x0 - x[i - 1]) / dx;
            return u[i - 1] + z * (u[i] - u[i - 1]);
        }

    private:
        static dvec linspace(double min, double max, unsigned n, bool verbose) {
            if (verbose)
                std::cout << "Initialising x" << std::endl;
            double d = (max - min) / n;
            dvec v(n);
            for (unsigned i = 0; i < n; ++i) {
                v[i] = min + i * d;
                if (verbose)
                    std::cout << "x" << i << ": " << v[i] << std::endl;
            }
            return v;
        }
    };
};


#endif //NUMLIB_SOLVER_H
