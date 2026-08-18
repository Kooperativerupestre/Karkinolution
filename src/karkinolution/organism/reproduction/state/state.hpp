#pragma once
#include "karkinolution/organism/reproduction/state/ontology.hpp"
#include "karkinolution/organism/stats.hpp"
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/reproduction/gestation.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <utility>
#include <variant>

using ReproductiveState = std::variant<std::monostate, Uterus>;
using OrganismStats::Health::Health;
using OrganismStats::Energy;

class ReproductionOrgan {
    public:

    ReproductiveState state;
    ReproductiveWays reproductive_way;

    ReproductionOrgan(ReproductiveState&&state, ReproductiveWays way) : state(state), reproductive_way(way) {}


    ReproductionOrgan(Gender gender, ReproductiveWays reproductive_way, Energy initial_energy, Health initial_health) {
        if (gender == Gender::MALE) {
            state = std::monostate();
        } else if (gender == Gender::FEMALE) {
            state = Uterus{EmptyUterus{}, initial_health, initial_energy};
        }
        std::unreachable();
    }
};