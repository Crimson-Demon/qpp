#ifndef NUMLIB_SOLVER_H
#define NUMLIB_SOLVER_H

#include <vector>
#include <cmath>
#include "pde.h"
#include "fdm.h"

namespace num {
    class PDESolver {
    public:
        dvec
        solve(const pde::BoundaryValueProblem &problem, const fdm::FDM *scheme, double tmin, double tmax, unsigned nt,
              double xmin, double xmax, unsigned nx) {
            dvec x = linspace(xmin, xmax, nx);
            for (unsigned i = 0; i <= nx; ++i)
                x[i] = problem.initialize(x[i]);
            double t = tmin, dt = (tmax - tmin) / nt, dx = (xmax - xmin) / nx;
            for (unsigned i = 0; i < nt; ++i) {
                x = scheme->step(problem, t, x, dt, dx);
                t += dt;
            }
            return x;
        }

    private:
        dvec linspace(double min, double max, unsigned n) const {
            double d = (max - min) / n;
            dvec v(n);
            for (unsigned i = 0; i < n; ++i)
                v[i] = min + i * d;
            return v;
        }
    };
};


#endif //NUMLIB_SOLVER_H
