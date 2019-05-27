#ifndef NUMLIB_SOLVER_H
#define NUMLIB_SOLVER_H

#include <vector>
#include "pde.h"
#include "fdm.h"

namespace num {

    class PDESolver {
    public:
        static std::vector<double> solve(const pde::PDE *pde, const fdm::FDM *scheme, double t0, const std::vector<double>& x0, double dt, double dx, unsigned steps) {
            std::vector<double> x = x0;
            double t = t0;
            for(unsigned i = 0; i < steps; ++i) {
                x = scheme->step(t, x, dt, dx);
            }
            return x;
        }
    };
};


#endif //NUMLIB_SOLVER_H
