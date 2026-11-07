#pragma once

#include "karkinolution/organism/entities/entities.hpp"
#include "shared/math/stats/compile_values.hpp"

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Physic {
				namespace Physiology {
					GreaterThanZeroValue<float> get_move_cost(const Entities::Entity &entity, const Vec2 &new_position);
				} // namespace Physiology
			} // namespace Physic
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution

namespace PhysicPhysiology = Karkinolution::Organisms::Entities::Physic::Physiology;