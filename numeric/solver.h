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
              double xmin, double x0, double xmax, unsigned nx) {
            dvec x = linspace(xmin, xmax, nx);
            dvec u(x.size());
            for (unsigned i = 0; i <= nx; ++i)
                u[i] = problem.initialize(x[i]);
            double t = tmin, dt = (tmax - tmin) / nt, dx = (xmax - xmin) / nx;
            for (unsigned i = 0; i < nt; ++i) {
                u = scheme->step(problem, u, t, x, dt, dx);
                t += dt;
            }
            unsigned i = 0;
            for (; i < nx; ++i) {
                if (x[i] > x0)
                    break;
            }
            double l = (x0 - x[i - 1]) / dx;
            return u[i - 1] + l * (u[i] - u[i - 1]);
        }

    private:
        static dvec linspace(double min, double max, unsigned n) {
            double d = (max - min) / n;
            dvec v(n);
            for (unsigned i = 0; i < n; ++i)
                v[i] = min + i * d;
            return v;
        }
    };
};


#endif //NUMLIB_SOLVER_H
