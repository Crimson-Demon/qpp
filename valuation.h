#ifndef QPP_VALUATION_H
#define QPP_VALUATION_H

#include <iostream>
#include "marketmodel.h"
#include "option.h"
#include "valuationutils.h"
#include "numeric/solver.h"
#include "simulation/montecarlo.h"
#include "simulation/simulation.h"
#include "simulation/random.h"

namespace qpp {

    double value(BSModel &m, Option &o, GenericValuationParameters<ValType::CF> &p) {
        double rate = m.get_rate(), volatility = m.get_vol();
        double spot = m.get_spot(
                o.get_underlying()), strike = o.get_strike(), timeToMaturity = o.get_time_to_maturity();
        double d1 = ((std::log(spot) / strike) + (rate + 0.5 * std::pow(volatility, 2)) * timeToMaturity) /
                    (volatility * std::sqrt(timeToMaturity));
        double d2 = d1 - volatility * std::sqrt(timeToMaturity);
        double optionSign = o.get_right() == OptionRight::CALL ? 1.00 : -1.00;
        return optionSign * rnd::NormalDistribution::std_cdf(optionSign * d1) * spot +
               (-optionSign) * rnd::NormalDistribution::std_cdf(optionSign * d2) * strike *
               std::exp(-rate * timeToMaturity);
    }

    // this can be generalised
    double value(BSModel &m, Option &o, GenericValuationParameters<ValType::MC> &p) {
        sim::Scheme *scheme = p.get_scheme();
        unsigned steps = p.get_steps();
        double s0 = m.get_spot(o.get_underlying());
        double dt = o.get_time_to_maturity() / steps;
        rnd::Distribution *distr = new rnd::NormalDistribution(0, std::sqrt(dt));
        sim::SDE sde = sim::SDEFactory::makeGBM(m.get_rate(), m.get_vol());
        std::function<double(rnd::BaseGenerator *)> callback = [&scheme, sde, distr, s0, dt, steps, &o](
                rnd::BaseGenerator *gen) -> double {
            return (*o.get_payoff())(sim::Simulation::trajectory(sde, scheme, gen, distr, s0, dt, steps));
        };
        mc::MonteCarloJob job(callback);
        mc::RunSettings settings = p.get_settings();
        mc::MonteCarloResult result = mc::MonteCarloEngine::run(job, settings);
        delete distr;
        return std::exp(-m.get_rate() * o.get_time_to_maturity()) * result.get_value();
    }

    double value(BSModel &m, Option &o, PDEParameters &p) {
        Payoff *payoff = o.get_payoff();
        double tmin = o.get_time_to_maturity();
        double tmax = 0;
        double xmin = 0;
        double x0 = m.get_spot(o.get_underlying());
        double xmax = std::max(2 * x0, 500.0);
        double r = m.get_rate();
        double v = m.get_vol();
        double K = o.get_strike();
        unsigned nt = p.get_nt(), nx = p.get_nx();

        std::function<double(double)> terminal = [payoff](double s) -> double { return (*payoff)(s); };
        std::function<double(double)> left = [](double t) -> double { return 0; };
        std::function<double(double)> right = [xmax, K, r, tmin](double t) -> double {
            return xmax - K * std::exp(-r * (tmin - t));
        };

        auto prob = pde::PDEFactory::makeBSPDE(r, v, terminal, left, right);
        auto settings = num::SolverSettings(tmin, tmax, nt, xmin, x0, xmax, nx);
        return num::PDESolver::solve(prob, settings, false);
    }
}

#endif //QPP_VALUATION_H
