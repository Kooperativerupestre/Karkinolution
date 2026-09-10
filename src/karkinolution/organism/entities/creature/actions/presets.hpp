#pragma once
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <variant>

struct MovePreset {
		Vec3 new_coord;
};

struct EatPreset {
		FoodHint                         hint;
		std::variant<EntityId, NatureId> id;
};

using AllPresets = std::variant<MovePreset, EatPreset>;