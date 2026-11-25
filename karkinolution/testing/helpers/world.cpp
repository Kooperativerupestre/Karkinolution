#include "karkinolution/testing/helpers/world.hpp"

#include "shared/math/units.hpp"

Karkinolution::World::WorldPreset Karkinolution::Testing::World::make_small_world_preset() {
	return {.seed    = 1,
			.size    = KShared::Math::Units::Size{.width  = KShared::Math::Units::Width{100.0},
												  .height = KShared::Math::Units::Height{100.0}},
			.epsilon = 5.0f};
}

Karkinolution::World::World Karkinolution::Testing::World::make_small_world() {
	const auto preset = make_small_world_preset();
	return {preset};
}
