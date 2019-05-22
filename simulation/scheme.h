#ifndef SIMLIB_SCHEME_H
#define SIMLIB_SCHEME_H

#include "sde.h"

// normally id implement simulate using trajectory, but it in the case of the direct simulation
// it kinda sucks to simulate a whole path, when you can just get the end result
// todo: look at idea of a stateful Monte Carlo Computation, that can be fluently chained
// todo: then a trajectory is just a simulation where the the starting value changes
// todo: assuming that each step is independant
class SimulationScheme {
public:
    virtual double simulate(SDE sde, BaseGenerator *g, double x0, double T, uint64_t steps) = 0;

    std::vector<double> simulate(const SDE &sde, BaseGenerator *g, const std::vector<double> &x0s, double T,
                                 uint64_t steps) {
        std::vector<double> result;
        for (auto const x0 : x0s) {
            result.push_back(simulate(sde, g, x0, T, steps));
        }
        return result;
    }

    virtual std::vector<double> trajectory(SDE sde, BaseGenerator *g, double x0, double T, uint64_t steps) = 0;

    virtual std::vector<std::vector<double>>
    trajectory(const SDE &sde, BaseGenerator *g, const std::vector<double> &x0s, double T,
               uint64_t steps) {
        std::vector<std::vector<double>> result;
        for (auto const x0 : x0s) {
            result.push_back(trajectory(sde, g, x0, T, steps));
        }
        return result;
    }
};

class DirectSimulationScheme : public SimulationScheme {
public:
    double simulate(SDE sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        double w = NormalDistribution::rand(g, 0.0, T);
        return sde.solve(x0, w, T);
    }

    std::vector<double> trajectory(SDE sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        std::vector<double> result(steps);
        return result;
    }
};

class EulerMaruyamaScheme : public SimulationScheme {
public:
    double simulate(SDE sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        return trajectory(sde, g, x0, T, steps).back();
    }

    std::vector<double> trajectory(SDE sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        double dt = T / steps, prev = x0, curr = 0, nPrev = 0 /*NormalDistribution::rand(g, 0.0, 1.0)*/, nCurr, dW;
        std::vector<double> result;
        result.push_back(prev);
        for (uint64_t i = 0; i < steps; ++i) {
            nCurr = NormalDistribution::rand(g, 0.0, std::sqrt(dt));
            dW = nCurr - nPrev;
            double an = sde.drift(prev, 0.0, 0.0);
            double bn = sde.diffusion(prev, 0.0, 0.0);
            curr = prev + an * dt + bn * dW; // todo: fix placeholder 0.0
            prev = curr;
            result.push_back(prev);
        }
        return result;
    }
};

// todo:
class MilsteinScheme : public SimulationScheme {
public:
    double simulate(SDE sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        return trajectory(sde, g, x0, T, steps).back();
    }

    std::vector<double> trajectory(SDE sde, BaseGenerator *g, double x0, double T, uint64_t steps) final {
        double dt = T /
                    steps, prev = x0, curr = 0, nPrev = 0 /*NormalDistribution::rand(g, 0.0, std::sqrt(dt))*/, nCurr, dW;
        std::vector<double> result;
        result.push_back(prev);
        for (uint64_t i = 0; i < steps; ++i) {
            nCurr = NormalDistribution::rand(g, 0.0, std::sqrt(dt));
            dW = nCurr - nPrev;
            double an = sde.drift(prev, 0.0, 0.0);
            double bn = sde.diffusion(prev, 0.0, 0.0);
            double dbn = sde.ddx_diffusion(prev, 0.0, 0.0);
            curr = prev + an * dt + bn * dW + 0.5 * bn * dbn * (std::pow(dW, 2) - dt); // todo: fix placeholder 0.0
            prev = curr;
            result.push_back(prev);
        }
        return result;
    }
};

#endif //SIMLIB_SCHEME_H
