#include "karkinolution/organism/entities/corpse/corpse.hpp"
#include "karkinolution/organism/foods/foods.hpp"
#include <karkinolution/organism/entities/corpse/physiology.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/core/id_generator.hpp>

Corpse CorpsePhysiology::generate_corpse(const Creature&creature) {
    RawMeat raw_meat{.energy = 0};
    
    const auto& metabolism = creature.body.metabolism;
    const auto& morphology = creature.body.morphology;

    raw_meat.energy += metabolism.energy.value() * 0.5f;
    raw_meat.energy += metabolism.reserved.value() * 0.75f;
    raw_meat.energy += morphology.volume().value * 0.20f * metabolism.reserved.value();
    return Corpse{gen_id(), raw_meat};
}

Corpse CorpsePhysiology::generate_corpse(const Embryo &embryo) {
    RawMeat raw_meat{.energy=0};

    raw_meat.energy += embryo.energy.value() * 0.4f;
    raw_meat.energy += embryo.volume.value * 0.15f * embryo.energy.value();
    return Corpse{gen_id(), raw_meat};
}

