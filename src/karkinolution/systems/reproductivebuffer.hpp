#pragma once
#include <karkinolution/organism/ontology.hpp>
#include <karkinolution/organism/genetics.hpp>
#include <karkinolution/core/basestorage.hpp>


struct ReproductiveDesire {
    const Id creature_id;
    const CreatureSpecies specie;
};

class ReproductiveBuffer : public BaseStorage<Id, ReproductiveDesire> {
    public:

    const ReproductiveDesire* try_at_by_specie(CreatureSpecies specie) const {
        return at_by_value([specie](Id id, ReproductiveDesire desire) { return desire.specie==specie; });
    }
};
