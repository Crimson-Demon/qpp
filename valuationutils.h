#ifndef QPP_VALUATIONUTILS_H
#define QPP_VALUATIONUTILS_H

#include "simulation/montecarlo.h"
#include "simulation/scheme.h"
#include "numeric/solver.h"

namespace qpp {

    enum class ValuationType {
        CF,
        MC,
        TREE,
        PDE
    };
    using ValType = ValuationType;
    using VT = ValuationType;

    class ValuationParameters {
    protected:
    public:
        virtual ~ValuationParameters() = 0;
    };

    inline ValuationParameters::~ValuationParameters() = default;

    template<ValuationType valType>
    class GenericValuationParameters : public ValuationParameters {
    public:
    };

    template<>
    class GenericValuationParameters<ValuationType::CF> : public ValuationParameters {
    public:
    };

    using CFParameters = GenericValuationParameters<ValuationType::CF>;

    template<>
    class GenericValuationParameters<ValuationType::MC> : public ValuationParameters {
        sim::Scheme *scheme;
        unsigned steps;
        mc::RunSettings settings;
    public:
        GenericValuationParameters() {
            scheme = new sim::EulerMaruyamaScheme();
            steps = 100;
        }

        ~GenericValuationParameters() final {
            delete scheme;
        }

        sim::Scheme *get_scheme() const { return scheme; }

        unsigned get_steps() const { return steps; }

        mc::RunSettings &get_settings() { return settings; }
    };

    using MCParameters = GenericValuationParameters<ValuationType::MC>;

    template<>
    class GenericValuationParameters<ValuationType::TREE> : public ValuationParameters {
    public:
    };

    using LatticeParameters = GenericValuationParameters<ValuationType::TREE>;

    template<>
    class GenericValuationParameters<ValuationType::PDE> : public ValuationParameters {
        unsigned nt;
        unsigned nx;
    public:
        GenericValuationParameters() : nt(20000), nx(20) {}

        GenericValuationParameters(unsigned nt, unsigned nx) : nt(nt), nx(nx) {}

        unsigned get_nt() const { return nt; }

        unsigned get_nx() const { return nx; }
    };

    using PDEParameters = GenericValuationParameters<ValuationType::PDE>;


}

#endif //QPP_VALUATIONUTILS_H
