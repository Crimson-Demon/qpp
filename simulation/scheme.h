#ifndef SIMLIB_SCHEME_H
#define SIMLIB_SCHEME_H

#include "sde.h"
#include "random.h"
#include <vector>

// todo: look at idea of a stateful Monte Carlo Computation, that can be fluently chained
// todo: then a trajectory is just a simulation where the the starting value changes
// todo: assuming that each step is independant
namespace sim {
    class Scheme {
    public:
        virtual double step(SDE sde, double x, double w, double dt) = 0;
    };

    class DirectScheme : public Scheme {
    public:
        double step(SDE sde, double x, double w, double dt) final {
            return sde.solve(x, w, dt);
        }
    };

    class EulerMaruyamaScheme : public Scheme {
    public:
        double step(SDE sde, double x, double dW, double dt) final {
            double an = sde.drift(x, 0.0, 0.0);
            double bn = sde.diffusion(x, 0.0, 0.0);
            return x + an * dt + bn * dW;
        }
    };

    class MilsteinScheme : public Scheme {
    public:
        double step(SDE sde, double x, double dW, double dt) final {
            double an = sde.drift(x, 0.0, 0.0);
            double bn = sde.diffusion(x, 0.0, 0.0);
            double dbn = sde.ddx_diffusion(x, 0.0, 0.0);
            return x + an * dt + bn * dW + 0.5 * bn * dbn * (std::pow(dW, 2) - dt);
        }
    };
}

#endif //SIMLIB_SCHEME_H