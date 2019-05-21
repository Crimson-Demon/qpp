#include <iostream>
#include "simulationutils.h"

int main() {
    unsigned seed = 100; //std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    BaseGenerator *g = new GenericGenerator<std::default_random_engine>(generator);
    std::cout << NormalDistribution::std_cdf(0) << std::endl;
    std::cout << NormalDistribution::std_rand(g) << std::endl;
    Distribution *d = new NormalDistribution(0, 1);
    g->seed(seed);
    std::cout << d->cdf(0) << std::endl;
    std::cout << d->rand(g) << std::endl;
    return 0;
}