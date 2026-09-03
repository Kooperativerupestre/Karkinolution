#pragma once

#include "karkinolution/math/physic/vec/model.hpp"

#include <cstdint>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/world/world.hpp>

enum class MoveOutput : uint8_t {
	SAME_POSITION,
	OK,
	ENTITY_MAP_MOTOR_FAILED
};

namespace MovementMotor {
	MoveOutput move(Creature &creature, World &world, const Vec3 &new_position); // atomic operation
} // namespace MovementMotor