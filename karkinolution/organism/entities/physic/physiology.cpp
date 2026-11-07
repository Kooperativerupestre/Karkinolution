#include "shared/math/stats/compile_values.hpp"

#include <karkinolution/organism/entities/physic/physiology.hpp>
#include <karkinolution/organism/internal_aliases.hpp>

KShared::Math::LV::CT::GreaterThanZeroValue<float> PhysicPhysiology::get_move_cost(const Entities::Entity &entity,
																				   const Vec2 &new_position) {
	return 1.0f;
}
