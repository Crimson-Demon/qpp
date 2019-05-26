#ifndef SIMLIB_RANDOM_H
#define SIMLIB_RANDOM_H

#include <random>

namespace rnd {

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

        virtual double inv_cdf(double value) = 0;

        virtual double rand(BaseGenerator *g) = 0;
    };

// Acklam's algorithm
    double inv_erf(double p) {
        // Coefficients in rational approximations.
        double a[6] = {-3.969683028665376e+01, 2.209460984245205e+02, -2.759285104469687e+02, 1.383577518672690e+02,
                       -3.066479806614716e+01, 2.506628277459239e+00};
        double b[5] = {-5.447609879822406e+01, 1.615858368580409e+02, -1.556989798598866e+02, 6.680131188771972e+01,
                       -1.328068155288572e+01};
        double c[6] = {-7.784894002430293e-03, -3.223964580411365e-01, -2.400758277161838e+00, -2.549732539343734e+00,
                       4.374664141464968e+00, 2.938163982698783e+00};
        double d[4] = {7.784695709041462e-03, 3.224671290700398e-01, 2.445134137142996e+00, 3.754408661907416e+00};

        // Define break-points.
        double p_low = 0.02425;
        double p_high = 1 - p_low;

        double q, x = 0.0;

        // Rational approximation for lower region.
        if (0 < p && p < p_low) {
            q = std::sqrt(-2 * std::log(p));
            x = (((((c[1] * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5]) * q + c[6]) /
                ((((d[1] * q + d[2]) * q + d[3]) * q + d[4]) * q + 1);
        }

        // Rational approximation for central region.
        if (p_low <= p && p <= p_high) {
            q = p - 0.5;
            double r = q * q;
            x = (((((a[1] * r + a[2]) * r + a[3]) * r + a[4]) * r + a[5]) * r + a[6]) * q /
                (((((b[1] * r + b[2]) * r + b[3]) * r + b[4]) * r + b[5]) * r + 1);
        }

        // Rational approximation for upper region.
        if (p_high < p && p < 1) {
            q = std::sqrt(-2 * std::log(1 - p));
            x = -(((((c[1] * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5]) * q + c[6]) /
                ((((d[1] * q + d[2]) * q + d[3]) * q + d[4]) * q + 1);
        }

        return x;
    }

// todo: cannot use double/float as template classes, look for reasonable workaround to template with double
// todo: and how to link this with inheriting from Distribution class
    class NormalDistribution : public Distribution {
        double mean;
        double variance;
        std::normal_distribution<double> distribution;

    public:
        NormalDistribution(double mean, double variance) : mean(mean), variance(variance),
                                                           distribution(
                                                                   std::normal_distribution<double>(mean, variance)) {}

        // todo: we really need a better approximation for normal_cdf than erf xD
        static double cdf(double value, double mean, double variance) {
            return 0.5 * (1 + erf((value - mean) /
                                  (variance * (M_SQRT1_2))));
        }

        static double inv_cdf(double value, double mean, double variance) {
            return M_SQRT1_2 * variance * inv_erf(2 * value - 1) + mean;
        }

        static double std_cdf(double value) { return cdf(value, 0.0, 1.0); }

        double cdf(double value) final { return cdf(value, mean, variance); }

        double inv_cdf(double value) final { return inv_cdf(value, mean, variance); }

        static double rand(BaseGenerator *g, double mean, double variance) {
            std::normal_distribution<double> localDistribution(mean, variance);
            return localDistribution(*g);
        }

        static double std_rand(BaseGenerator *g) { return rand(g, 0.0, 1.0); }

        double rand(BaseGenerator *g) final { return distribution(*g); }
    };
}

#endif //SIMLIB_RANDOM_H