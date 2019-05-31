#include <iostream>
#include "solver.h"

int main() {
    using namespace pde;
    using namespace fdm;
    using namespace num;

    double s0 = 100, K = 100, r = 0.05, v = 0.2, T = 1;
    double smin = 90, smax = 120;
    unsigned nt = 100, ns = 100;
    std::function<double(double)> payoff = [K](double s) -> double { return std::max(s - K, 0.0); };
    std::function<double(double)> left = [](double t) -> double { return 0; };
    std::function<double(double)> right = [smax](double t) -> double { return smax; };

    auto prob = PDEFactory::makeBSPDE(r, v, payoff, left, right);
    auto* fdm = new ExplicitScheme();

    double res = PDESolver::solve(prob, fdm, 0.0, T, nt, smin, s0, smax, ns);

    std::cout << res << std::endl;

    delete fdm;

    return 0;
}