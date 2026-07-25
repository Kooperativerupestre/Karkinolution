#include <karkinolution/core/error.hpp>
#include <karkinolution/organism/genetics.hpp>
#include <cassert>
#include <karkinolution/organism/creatures.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <format>

namespace Validator {
    inline void validate_female_gender(const Creature&creature) {
        assert(creature.gender == Gender::FEMALE);
    }

    inline void validate_male_gender(const Creature&creature) {
        assert(creature.gender == Gender::MALE);
    }
    inline void validate_pregnant(const Creature&creature) {
        assert(creature.pregnant());
    }
    inline void validate_non_pregnant(const Creature&creature) {
        assert(!creature.pregnant());
    }
    inline void validate_same_species(CreatureSpecies A, CreatureSpecies B) {
        assert(A == B);
    }
    inline void validate_different_genders(Gender A, Gender B) {
        assert(A == B);
    }
    inline void validate_id_type_is_creature(Id id) {
        assert(id.entity_type == EntityTypes::CREATURE);
    }
    inline void validate_id_type_is_corpse(Id id) {
        assert(id.entity_type == EntityTypes::CORPSE);
    }
    template <typename T>
    inline void check_energy(const Creature&creature, T energy) {
        if (creature.energy.value() < energy) {
            throw InsufficientEnergyError(std::format("insufficient energy error: creature energy {} < {}", creature.energy.value(), energy));
        }
    }
    inline void validate_reproductive_capacity(const Creature&creature) {
        assert(creature.reproductively_capable());
    }
}