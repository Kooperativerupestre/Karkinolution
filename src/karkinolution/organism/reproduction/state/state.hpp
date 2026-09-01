#pragma once
#include "karkinolution/organism/reproduction/state/ontology.hpp"
#include "karkinolution/organism/stats.hpp"
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/reproduction/gestation.hpp>
#include <karkinolution/organism/reproduction/uterus/uterus.hpp>
#include <utility>
#include <variant>

using ReproductiveOrgan = std::variant<std::monostate, Uterus>;
using OrganismStats::Energy;
using OrganismStats::Health::Health;

class ReproductiveState {
  public:
    ReproductiveOrgan state;
    ReproductiveWays reproductive_way;

    bool is_pregnant() const {
        if (std::holds_alternative<std::monostate>(state)) {
            return false;
        } else {
            return (std::get<Uterus>(state).is_pregnant());
        }
    }

    ReproductiveState(ReproductiveOrgan&& state, ReproductiveWays way)
        : state(state), reproductive_way(way) {}

    ReproductiveState(Gender gender, ReproductiveWays reproductive_way, Energy initial_energy,
                      Health initial_health) {
        if (gender == Gender::MALE) {
            state = std::monostate();
        } else if (gender == Gender::FEMALE) {
            state = Uterus{EmptyUterus{}, initial_health, initial_energy};
        }
        std::unreachable();
    }
};