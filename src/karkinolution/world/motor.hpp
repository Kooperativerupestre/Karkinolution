#pragma once
#include <karkinolution/world/world.hpp>

namespace WorldMotor {
	void remove(EntityId id, World &world);
	bool add(Entity &&entity, World &world);
} // namespace WorldMotor
