#pragma once

#include "karkinolution/organism/entities/physic/analyzer.hpp"
#include "karkinolution/terrain/terrain.hpp"
#include "karkinolution/world/world.hpp"

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Physic {
				namespace Motor {
					Analyzer::CanBeOutput
					move(EntityId id, const Vec3 &new_position, Karkinolution::World::World &world);
				} // namespace Motor
			} // namespace Physic
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution

namespace PhysicMotor = Karkinolution::Organisms::Entities::Physic::Motor;