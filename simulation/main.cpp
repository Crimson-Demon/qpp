#include <iostream>
#include "simulation.h"
#include "montecarlo.h"

int main() {
    using namespace sim;
    using namespace mc;
    using namespace rnd;

    unsigned seed = 100; //std::chrono::system_clock::now().time_since_epoch().count();
    BaseGenerator *g = GeneratorFactory<std::mt19937>::get();
    g->seed(seed);

    std::cout << NormalDistribution::std_cdf(0) << std::endl;
    std::cout << NormalDistribution::std_rand(g) << std::endl;

    Distribution *d = new NormalDistribution(0, 1);
    g->seed(seed);
    std::cout << d->cdf(0) << std::endl;
    std::cout << d->rand(g) << std::endl;

    double mu = 0.05, sigma = 0.25;
    SDE gbm = SDEFactory::makeGBM(mu, sigma);
    Scheme *scheme1 = new EulerMaruyamaScheme();
    Scheme *scheme2 = new MilsteinScheme();
    Scheme *scheme3 = new DirectScheme();

    unsigned steps = 4368, paths = 10000;
    double T = 1, x0 = 100, K = 100, dt = T / steps;
    double alpha = 0.95;

    Distribution *distr1 = new NormalDistribution(0, std::sqrt(dt));
    Distribution *distr2 = new NormalDistribution(0, T);

    // todo: decouple scheme from callback... idk, to make it more customizable in the settings or smth
    // todo: idea - > have the callback have the scheme as an argument
    std::function<double(BaseGenerator *)> callback1 = [&scheme1, &gbm, &distr1, x0, K, dt, steps](
            BaseGenerator *gen) -> double {
        return std::max(Simulation::trajectory(gbm, scheme1, gen, distr1, x0, dt, steps) - K, 0.0);
    };
    std::function<double(BaseGenerator *)> callback2 = [&scheme2, &gbm, &distr1, x0, K, dt, steps](
            BaseGenerator *gen) -> double {
        return std::max(Simulation::trajectory(gbm, scheme2, gen, distr1, x0, dt, steps) - K, 0.0);
    };
    std::function<double(BaseGenerator *)> callback3 = [&scheme3, &gbm, &distr2, x0, K, T](
            BaseGenerator *gen) -> double {
        return std::max(Simulation::trajectory(gbm, scheme3, gen, distr2, x0, T, 1) - K, 0.0);
    };

    RunSettings settings1(RunType::SEQUENTIAL, g, paths, alpha);
    RunSettings settings2(RunType::CONCURRENT, g, paths, alpha);

    MonteCarloJob job1(callback1);
    MonteCarloJob job2(callback2);
    MonteCarloJob job3(callback3);

    g->seed(seed);
    auto result = MonteCarloEngine::run(job1, settings1);
    std::cout << "sequential euler maruyama result: " << std::exp(-mu * T) * result.getValue() << " time: "
              << result.getDuration().count() << "ms" << std::endl;

    g->seed(seed);
    result = MonteCarloEngine::run(job2, settings1);
    std::cout << "sequential milstein result: " << std::exp(-mu * T) * result.getValue() << " time: "
              << result.getDuration().count() << "ms" << std::endl;

    g->seed(seed);
    result = MonteCarloEngine::run(job3, settings1);
    std::cout << "sequential direct simulation result: " << std::exp(-mu * T) * result.getValue() << " time: "
              << result.getDuration().count() << "ms" << std::endl;

    g->seed(seed);
    result = MonteCarloEngine::run(job1, settings2);
    std::cout << "concurrent euler maruyama result: " << std::exp(-mu * T) * result.getValue() << " time: "
              << result.getDuration().count() << "ms" << std::endl;

    g->seed(seed);
    result = MonteCarloEngine::run(job2, settings2);
    std::cout << "concurrent milstein result: " << std::exp(-mu * T) * result.getValue() << " time: "
              << result.getDuration().count() << "ms" << std::endl;

    g->seed(seed);
    result = MonteCarloEngine::run(job3, settings2);
    std::cout << "concurrent direct simulation result: " << std::exp(-mu * T) * result.getValue() << " time: "
              << result.getDuration().count() << "ms" << std::endl;
    return 0;
}