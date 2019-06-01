#include <iostream>
#include "solver.h"

int main() {
    using namespace pde;
    using namespace fdm;
    using namespace num;

    bool verbose = false;

    double s0 = 20, K = 10, r = 0.05, v = 0.25, tmin = 1, tmax = 0;
    double smin = 10, smax = 500;
    unsigned nt = 100000, ns = 1000;
    std::function<double(double)> payoff = [K](double s) -> double { return std::max(s - K, 0.0); };
    std::function<double(double)> left = [](double t) -> double { return 0; };
    std::function<double(double)> right = [smax, K, r, tmin](double t) -> double {
        return smax - K * std::exp(-r * (tmin - t));
    };

    auto prob = PDEFactory::makeBSPDE(r, v, payoff, left, right);
    auto *fdm = new ExplicitScheme();

    double res = PDESolver::solve(prob, fdm, tmin, tmax, nt, smin, s0, smax, ns, verbose);

    std::cout << "BS PDE result is " << res << std::endl;

    delete fdm;

    auto bs_space_trans = [r, v, tmin](double s, double t) -> double {
        return std::log(s) + (r - 0.5 * v * v) * (tmin - t);
    };
    auto bs_time_trans = [v, tmin](double t) -> double { return 0.5 * v * v * (tmin - t); };

    double x0 = bs_space_trans(s0, tmin), taumin = bs_time_trans(tmin), taumax = bs_time_trans(tmax);
    double xmin = bs_space_trans(smin, tmin), xmax = bs_space_trans(smax, tmin);
    unsigned nx = ns;
    payoff = [K, r, tmin](double x) -> double { return std::exp(-r * tmin) * std::max(std::exp(x) - K, 0.0); };
    left = [](double tau) -> double { return 0; };
    right = [r, tmin, v, xmax](double tau) -> double { return std::exp(-r * (tmin - 2 * tau / (v * v))) * xmax; };
    auto diffusion = [](double t, double x) -> double { return 1; };

    prob = PDEFactory::makeHeatPDE(diffusion, payoff, left, right);
    fdm = new ExplicitScheme();

    res = PDESolver::solve(prob, fdm, taumin, taumax, nt, xmin, x0, xmax, nx, verbose);

    std::cout << "Heat PDE result is " << res << std::endl;

    delete fdm;

    return 0;
}