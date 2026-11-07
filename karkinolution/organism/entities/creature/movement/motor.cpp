#include "shared/math/physic/vec/model.hpp"

#include <iostream>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/movement/motor.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Organisms::Entities::Creature::Movement::Motor {
	MoveOutput MovementMotor::move(Creature &creature, Karkinolution::World::World &world, const Vec2 &new_position) {
		if (creature.position == new_position) {
			std::cerr << "Moving a creature to the same position" << "\n";
			return MoveOutput::SAME_POSITION;
		}

		const auto was_updated = world.update(creature.build_id(), new_position);
		if (!was_updated) {
			return MoveOutput::ENTITY_MAP_MOTOR_FAILED;
		}

		creature.position = new_position;
		return MoveOutput::OK;
	}
} // namespace Karkinolution::Organisms::Entities::Creature::Movement::Motor
