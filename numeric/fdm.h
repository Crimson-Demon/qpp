#ifndef NUMLIB_FDM_H
#define NUMLIB_FDM_H


#include <functional>

#include "pde.h"

namespace fdm {

    enum class FiniteDifferenceType {
        FORWARD, BACKWARD, CENTRAL
    };

    std::pair<double, double> get_bump(FiniteDifferenceType type, double db) {
        double b1 =
                type == FiniteDifferenceType::FORWARD ? db : type == FiniteDifferenceType::BACKWARD ? 0 : 0.5 *
                                                                                                          db;
        double b2 = b1 - db;
        return std::make_pair(b1, b2);
    }

    template<unsigned dim>
    double *arr_sum(double x[dim], double y[dim]) {
        double z[dim];
        for (unsigned i = 0; i < dim; ++i)
            z[i] = x[i] + y[i];
        return z;
    }

    template<unsigned dim>
    double *arr_mul(double x[dim], double y) {
        double z[dim];
        for (unsigned i = 0; i < dim; ++i)
            z[i] = x[i] * y;
        return z;
    }

    class FiniteDifference {
    };

    template<unsigned order, unsigned dim, FiniteDifferenceType type>
    class GenericFiniteDifference : public FiniteDifference {
        GenericFiniteDifference() = default;

    public:
        static double
        value(const std::function<double(double, double[dim])> &f, double t, double x[dim], bool i[dim + 1], double dt,
              double dx) {
            auto[dt1, dt2] = get_bump(type, dt);
            auto[dx1, dx2] = get_bump(type, dx);
            double x1[dim] = arr_sum(arr_mul(arr_mul(i, i[0]), dx1), x);
            double x2[dim] = arr_sum(arr_mul(arr_mul(i, i[0]), dx2), x);
            double t1 = t + dt1 * i[0];
            double t2 = t + dt2 * i[0];
            return (GenericFiniteDifference<order - 1, dim, type>::value(f, t1, x1, i, dt, dx) -
                    GenericFiniteDifference<order - 1, dim, type>::value(f, t2, x2, i, dt, dx)) / (i[0] ? dt : dx);
        }
    };

    template<unsigned dim, FiniteDifferenceType type>
    class GenericFiniteDifference<0, dim, type> {
        GenericFiniteDifference() = default;

    public:
        static double
        value(const std::function<double(double, double[dim])> &f, double t, double x[dim], bool i[dim + 1], double dt,
              double dx) {
            return f(t, x);
        }
    };

    class FDM {
    public:
        virtual std::vector<double> step(double t0, std::vector<double> x0, double dt, double dx) const = 0;
    };

    template<unsigned dim>
    class ExplicitScheme : public FDM {
    public:
        std::vector<double> step(double t0, std::vector<double> x0, double dt, double dx) const {
            std::
        }
    };

    template<unsigned dim>
    class ImplicitScheme : public FDM {

    };

    template<unsigned dim>
    class CrankNicholsonScheme : public FDM {

    };
};


#endif //NUMLIB_FDM_H
