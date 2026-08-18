#pragma once
#include "karkinolution/organism/reproduction/state/ontology.hpp"
#include <cassert>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <karkinolution/organism/reproduction/state/state.hpp>
#include <variant>


namespace ReproductionValidator {
    inline void has_uterus(const ReproductionOrgan&reproduction) {
        assert(std::holds_alternative<Uterus>(reproduction.state));
    }
    inline void does_not_have_uterus(const ReproductionOrgan&reproduction) {
        assert(std::holds_alternative<std::monostate>(reproduction.state));
    }
    inline void is_pregnant(const ReproductionOrgan&reproduction) {
        has_uterus(reproduction);

        assert(std::get<Uterus>(reproduction.state).is_pregnant());
    }

    inline void is_not_pregnant(const ReproductionOrgan&reproduction) {
        has_uterus(reproduction);
        assert(!std::get<Uterus>(reproduction.state).is_pregnant());
    }

    inline void is_oviparous(const ReproductionOrgan&reproduction) {
        assert(reproduction.reproductive_way == ReproductiveWays::OVIPAROUS);
    }
    inline void is_viviparous(const ReproductionOrgan&reproduction) {
        assert(reproduction.reproductive_way == ReproductiveWays::VIVIPAROUS);
    }
}