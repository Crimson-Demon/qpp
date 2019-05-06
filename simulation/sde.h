#ifndef SIMLIB_SDE_H
#define SIMLIB_SDE_H

#include <functional>

#include "random.h"

// todo: too bad currying doesnt exist here, as an SDESolution is an Generator->Double3Func
// todo: maybe Ill actually incorporate that observation?!
typedef std::function<double(BaseGenerator*, double, double)> SDESolution;
typedef std::function<double(double, double)> Double3Function;

// todo: maybe template by return type and vector space or smth like that
class SDE {
    SDESolution solution;
    Double3Function quadraticVariationFunc;
    Double3Function driftFunc;
    Double3Function diffusionFunc;
public:
    SDE(SDESolution solution, Double3Function quadraticVariation, Double3Function drift, Double3Function diffusion) :
        solution(std::move(solution)),
        quadraticVariationFunc(std::move(quadraticVariation)),
        driftFunc(std::move(drift)),
        diffusionFunc(std::move(diffusion))
    { }

    double solve(BaseGenerator *g, double x, double t) { return this->solution.operator()(g, x, t); }
    std::vector<double> solve(BaseGenerator* g, const std::vector<double>& xs, double t) {
        std::vector<double> result;
        for(auto x : xs)
            result.push_back(solve(g, x, t));
        return result;
    }

    double quadraticVariation(double x, double t) { return this->quadraticVariationFunc.operator()(x, t); }
    std::vector<double> quadraticVariation(const std::vector<double>& xs, double t) {
        std::vector<double> result;
        for(auto x : xs)
            result.push_back(quadraticVariation(x, t));
        return result;
    }

    double drift(double x, double t) { return this->driftFunc.operator()(x, t); }
    std::vector<double> drift(const std::vector<double>& xs, double t) {
        std::vector<double> result;
        for(auto x : xs)
            result.push_back(drift(x, t));
        return result;
    }

    double diffusion(double x, double t) { return this->diffusionFunc.operator()(x, t); }
    std::vector<double> diffusion(const std::vector<double>& xs, double t) {
        std::vector<double> result;
        for(auto x : xs)
            result.push_back(diffusion(x, t));
        return result;
    }
};

// todo: if we had a static initialized map from the SDE type to the function instances, that would be nice
// todo: we could then template using some SDE enum that would be mapped to the functions
class SDEFactory {
public:
    static SDE makeGBM(double mu, double sigma) {
        auto solution = [mu, sigma](BaseGenerator* g, double x, double t)->double {
            return x * std::exp((mu - std::pow(sigma, 2) * 0.5) * t + sigma * NormalDistribution::rand(g, 0.0, t));
        };
        auto quadraticVariation = [sigma](double x, double t)->double { return std::pow(sigma * x, 2); };
        auto drift = [mu](double x, double t)->double { return x*mu; } ;
        auto diffusion = [sigma](double x, double t)->double { return x*sigma; };
        SDE sde(solution, quadraticVariation, drift, diffusion);
        return sde;
    }
};

#endif //SIMLIB_SDE_H
