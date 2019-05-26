#ifndef SIMLIB_SIMULATION_H
#define SIMLIB_SIMULATION_H

#include "sde.h"
#include "scheme.h"

namespace sim {
    class Simulation {
    public:
        static double
        trajectory(const SDE &sde, Scheme *scheme, rnd::BaseGenerator *g, rnd::Distribution *d, double x0, double dt,
                   unsigned steps) {
            double x = x0, w;
            for (unsigned i = 0; i < steps; ++i) {
                w = d->rand(g);
                x = scheme->step(sde, x, w, dt);
            }
            return x;
        }
    };
}

#endif //SIMLIB_SIMULATION_H