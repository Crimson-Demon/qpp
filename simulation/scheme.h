#ifndef SIMLIB_SCHEME_H
#define SIMLIB_SCHEME_H

#include "sde.h"

class SimulationScheme {
public:
    virtual double trajectory(SDE *sde, BaseGenerator *g, double x0, double T, uint64_t steps) = 0;
};

class DirectSimulationScheme : public SimulationScheme {
public:
    double trajectory(SDE *sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        return sde->solve(g, x0, T);
    }
};

class EulerMaruyamaScheme : public SimulationScheme {
public:
    double trajectory(SDE *sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        double dt = T / steps, prev = x0, curr, nPrev = NormalDistribution::rand(g, 0.0, 1.0), nCurr, dW;
        for(uint64_t i = 0; i < steps; ++i) {
            nCurr = NormalDistribution::rand(g, 0.0, 1.0);
            dW = nCurr - nPrev;
            curr = prev + sde->drift(prev, 0.0) * dt + sde->diffusion(prev, 0.0) * dW; // todo: fix placeholder 0.0
            prev = curr;
        }
        return curr;
    }
};

// todo:
class MilsteinScheme : public SimulationScheme {
public:
    double trajectory(SDE *sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        return 0.0;
    }
};


#endif //SIMLIB_SCHEME_H
