#pragma once
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/foods/foods.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <variant>

struct MovePreset {
		Vec3 new_coord;
};

struct EatPreset {
		FoodHint                      hint;
		std::variant<Id, SoilPieceId> id;
};

using AllPresets = std::variant<MovePreset, EatPreset>;