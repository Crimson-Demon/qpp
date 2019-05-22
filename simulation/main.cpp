#include <iostream>
#include "simulationutils.h"
#include "sde.h"
#include "scheme.h"

typedef std::chrono::milliseconds TimeT;

int main() {
    unsigned seed = 100; //std::chrono::system_clock::now().time_since_epoch().count();
    BaseGenerator *g = GeneratorFactory<std::mt19937>::get();
    g->seed(seed);

    std::cout << NormalDistribution::std_cdf(0) << std::endl;
    std::cout << NormalDistribution::std_rand(g) << std::endl;

    Distribution *d = new NormalDistribution(0, 1);
    g->seed(seed);
    std::cout << d->cdf(0) << std::endl;
    std::cout << d->rand(g) << std::endl;

    SDE gbm = SDEFactory::makeGBM(0.05, 0.25);
    SimulationScheme *scheme1 = new EulerMaruyamaScheme();
    SimulationScheme *scheme2 = new MilsteinScheme();
    SimulationScheme *scheme3 = new DirectSimulationScheme();

    unsigned steps = 1000;
    double T = 1, x0 = 10;

    using namespace sim;

    // todo: decouple scheme from callback... idk, to make it more customizable in the settings or smth
    // todo: idea - > have the callback have the scheme as an argument
    std::function<double(BaseGenerator *)> callback1 = [&scheme1, &gbm, x0, T, steps](
            BaseGenerator *gen) -> double { return scheme1->simulate(gbm, gen, x0, T, steps); };
    std::function<double(BaseGenerator *)> callback2 = [&scheme2, &gbm, x0, T, steps](
            BaseGenerator *gen) -> double { return scheme2->simulate(gbm, gen, x0, T, steps); };
    std::function<double(BaseGenerator *)> callback3 = [&scheme3, &gbm, x0, T, steps](
            BaseGenerator *gen) -> double { return scheme3->simulate(gbm, gen, x0, T, steps); };
    RunSettings settings1(RunType::SEQUENTIAL, g, 10000);
    RunSettings settings2(RunType::CONCURRENT, g, 10000);
    MonteCarloJob job1(callback1);
    MonteCarloJob job2(callback2);
    MonteCarloJob job3(callback3);

    // todo: add time measuring for jobs in the engine and have it in the result wrapper
    g->seed(seed);
    auto start = std::chrono::steady_clock::now();
    auto result = MonteCarloEngine::run(job1, settings1);
    auto duration = std::chrono::duration_cast<TimeT>(std::chrono::steady_clock::now() - start);
    std::cout << "sequential euler maruyama result: " << result << " time: " << duration.count() << "ms" << std::endl;

    g->seed(seed);
    start = std::chrono::steady_clock::now();
    result = MonteCarloEngine::run(job2, settings1);
    duration = std::chrono::duration_cast<TimeT>(std::chrono::steady_clock::now() - start);
    std::cout << "sequential milstein result: " << result << " time: " << duration.count() << "ms" << std::endl;

    g->seed(seed);
    start = std::chrono::steady_clock::now();
    result = MonteCarloEngine::run(job3, settings1);
    duration = std::chrono::duration_cast<TimeT>(std::chrono::steady_clock::now() - start);
    std::cout << "sequential direct simulation result: " << result << " time: " << duration.count() << "ms" << std::endl;

    g->seed(seed);
    start = std::chrono::steady_clock::now();
    result = MonteCarloEngine::run(job1, settings2);
    duration = std::chrono::duration_cast<TimeT>(std::chrono::steady_clock::now() - start);
    std::cout << "concurrent euler maruyama result: " << result << " time: " << duration.count() << "ms" << std::endl;

    g->seed(seed);
    start = std::chrono::steady_clock::now();
    result = MonteCarloEngine::run(job2, settings2);
    duration = std::chrono::duration_cast<TimeT>(std::chrono::steady_clock::now() - start);
    std::cout << "concurrent milstein result: " << result << " time: " << duration.count() << "ms" << std::endl;

    g->seed(seed);
    start = std::chrono::steady_clock::now();
    result = MonteCarloEngine::run(job3, settings2);
    duration = std::chrono::duration_cast<TimeT>(std::chrono::steady_clock::now() - start);
    std::cout << "concurrent direct simulation result: " << result << " time: " << duration.count() << "ms" << std::endl;
    return 0;
}