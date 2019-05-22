#ifndef SIMLIB_RANDOM_H
#define SIMLIB_RANDOM_H

#include <random>

class BaseGenerator {
public:
    virtual uint operator()() = 0;

    virtual void seed(unsigned long seed) = 0;

    // todo: those should be static constexpr - check if they wont break
    virtual uint min() = 0;

    virtual uint max() = 0;
};

// todo: add generic seeding logic
template<typename Generator>
class GenericGenerator : public BaseGenerator {
    Generator generator;
public:
    GenericGenerator(Generator &g) : generator(g) {}

    uint operator()() final { return generator(); }

    void seed(unsigned long seed) final { generator.seed(seed); }

    uint min() final { return generator.min(); }

    uint max() final { return generator.max(); }
};

template<typename Generator>
class GeneratorFactory {
public:
    static BaseGenerator *get() {
        Generator generator(69);
        BaseGenerator *bg = new GenericGenerator<Generator>(generator);
        return bg;
    }
};

class Distribution {
public:
    virtual double cdf(double value) = 0;

    virtual double rand(BaseGenerator *g) = 0;
};

// todo: cannot use double/float as template classes, look for reasonable workaround to template with double
// todo: and how to link this with inheriting from Distribution class
class NormalDistribution : public Distribution {
    double mean;
    double variance;
    std::normal_distribution<double> distribution;

public:
    NormalDistribution(double mean, double variance) : mean(mean), variance(variance),
                                                       distribution(std::normal_distribution<double>(mean, variance)) {}

    // todo: we really need a better approximation for normal_cdf than erf xD
    static double cdf(double value, double mean, double variance) {
        return 0.5 * (1 + erf((value - mean) /
                              (variance * (M_SQRT1_2))));
    }

    static double std_cdf(double value) { return cdf(value, 0.0, 1.0); }

    double cdf(double value) final { return cdf(value, mean, variance); }

    static double rand(BaseGenerator *g, double mean, double variance) {
        std::normal_distribution<double> localDistribution(mean, variance);
        return localDistribution(*g);
    }

    static double std_rand(BaseGenerator *g) { return rand(g, 0.0, 1.0); }

    double rand(BaseGenerator *g) final { return distribution(*g); }
};


#endif //SIMLIB_RANDOM_H
