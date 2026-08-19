#pragma once
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>

namespace CorpsePhysiology {
    Corpse generate_corpse(const Creature&creature);
    Corpse generate_corpse(const Embryo&embryo);
}