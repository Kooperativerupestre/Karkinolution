#pragma once
#include "karkinolution/organism/entities/creature/brain/instincts/instincts.hpp"

#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/world/world.hpp>

namespace BrainMotor {
	void run(Brain &brain, const Creature &creature);
} // namespace BrainMotor

namespace CreatureMotor {
	void          run(Creature &creature, World &);
	void          update_map(const Creature &creature, World &world);
	void          grow(Creature &creature, const OrganismRegistry &);
	PlannerOutput resolve_presets(Creature &creature, const Perception &perception, World &world);
} // namespace CreatureMotor
