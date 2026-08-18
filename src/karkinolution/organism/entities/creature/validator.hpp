#pragma once
#include <cassert>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>

namespace CreatureValidator {
    inline void is_female(const Creature&creature) {
        assert(creature.ontology.gender == Gender::FEMALE);
    }

    inline void is_male(const Creature&creature) {
        assert(creature.ontology.gender == Gender::MALE);
    }

    inline void is_same_specie(const Creature&A, const Creature&B) {
        assert(A.genome.core_genome.specie == B.genome.core_genome.specie);
    }

    inline void is_different_specie(const Creature&A, const Creature&B) {
        assert(A.genome.core_genome.specie != B.genome.core_genome.specie);
    }
}