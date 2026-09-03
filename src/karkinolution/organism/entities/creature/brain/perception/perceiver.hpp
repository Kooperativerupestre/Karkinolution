#pragma once

#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/brain/perception/perception.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/world/world.hpp>

namespace Perceiver {
PerceivedCreature perceive(const Creature &target, const Creature &perceiver);
PerceivedCorpse   perceive(const Corpse &target, const Creature &perceiver);
PerceivedSoil     perceive(const SoilPiece &soil, const Creature &perceiver);

Perception perceive(const Creature &perceiver, const World &world);
} // namespace Perceiver