#pragma once
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <variant>

namespace Karkinolution::Organisms::Entities::Creature::Actions {

	struct MovePreset {
			Karkinolution::Math::Vec::Vec2 new_coord;
	};

	struct EatPreset {
			Karkinolution::Organisms::FoodHint                                                                     hint;
			std::variant<Karkinolution::Organisms::Entities::EntityId, Karkinolution::Organisms::Nature::NatureId> id;
	};

	using AllPresets = std::variant<MovePreset, EatPreset>;

} // namespace Karkinolution::Organisms::Entities::Creature::Actions
