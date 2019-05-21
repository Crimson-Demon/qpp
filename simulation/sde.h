#ifndef SIMLIB_SDE_H
#define SIMLIB_SDE_H

#include <functional>

#include "random.h"

// todo: too bad currying doesnt exist here, as an SDESolution is an Generator->Double3Func
// todo: maybe Ill actually incorporate that observation?!
typedef std::function<double(double, double, double)> SDEFunction;

// todo: maybe template by return type and vector space or smth like that
class SDE {
    SDEFunction solutionFunc;
    SDEFunction quadVarFunc;
    SDEFunction driftFunc;
    SDEFunction diffusionFunc;
public:
    SDE(SDEFunction solution, SDEFunction quadraticVariation, SDEFunction drift, SDEFunction diffusion) :
            solutionFunc(std::move(solution)),
            quadVarFunc(std::move(quadraticVariation)),
            driftFunc(std::move(drift)),
            diffusionFunc(std::move(diffusion)) {}

    double solve(double x, double w, double t) { return this->solutionFunc.operator()(x, w, t); }

    double quadVar(double x, double w, double t) { return this->quadVarFunc.operator()(x, w, t); }

    double drift(double x, double w, double t) { return this->driftFunc.operator()(x, w, t); }

    double diffusion(double x, double w, double t) { return this->diffusionFunc.operator()(x, w, t); }
};

// todo: if we had a static initialized map from the SDE type to the function instances, that would be nice
// todo: we could then template using some SDE enum that would be mapped to the functions
class SDEFactory {
public:
    static SDE makeGBM(double mu, double sigma) {
        auto solution = [mu, sigma](double x, double w, double t) -> double {
            return x * std::exp((mu - std::pow(sigma, 2) * 0.5) * t + sigma * w);
        };
        auto quadVar = [sigma](double x, double w, double t) -> double { return std::pow(sigma * x, 2); };
        auto drift = [mu](double x, double w, double t) -> double { return x * mu; };
        auto diffusion = [sigma](double x, double w, double t) -> double { return x * sigma; };
        SDE sde(solution, quadVar, drift, diffusion);
        return sde;
    }
};

#endif //SIMLIB_SDE_H
