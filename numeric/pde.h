#ifndef NUMLIB_PDE_H
#define NUMLIB_PDE_H

#include <functional>
#include <vector>

namespace pde {

    template<unsigned order>
    class DifferentialOperator {
        unsigned indices[order];
    public:
        DifferentialOperator() = default;

        DifferentialOperator(const unsigned indices[order]) : indices(indices) {}

        void set(unsigned ind, unsigned val) { indices[ind] = val; }
    };

    template<unsigned order>
    using DiffOp = DifferentialOperator<order>;

    template<unsigned order, unsigned dim>
    class PDEComponent {
        double constant;
        std::function<double(double, double[dim])> auxilliary;
        DiffOp<order> op;
    public:
        double get_constant() const { return constant; }

        std::function<double(double, double[dim])> get_auxilliary() const { return auxilliary; }

        DiffOp<order> get_op() const { return op; }
    };

    template<unsigned rank, unsigned order[rank], unsigned dim>
    class PDEComponents {
        PDEComponent<order[0], dim> component;
        PDEComponents<rank - 1, &order[1], dim> tail;
    public:
        PDEComponent<order[0], rank> get_component(unsigned i) {
            return i == 0 ? component : tail.get_component(i - 1);
        }
    };

    template<unsigned order[1], unsigned dim>
    class PDEComponents<1, order, dim> {
        PDEComponent<1, dim> component;
    public:
        PDEComponent<1, dim> get_component(unsigned i) {
            return component;
        }
    };

    class PDE {

    };

    template<unsigned rank, unsigned order[rank], unsigned dim>
    class GenericPDE : public PDE {
        PDEComponents<rank, order, dim> components;
    public:
        GenericPDE() {}
        PDEComponent<order, dim> get_component(unsigned i) { return components.get_component(i); }
    };
};

#endif //NUMLIB_PDE_H
