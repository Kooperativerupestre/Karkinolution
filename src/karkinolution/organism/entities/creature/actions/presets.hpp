#pragma once
#include <karkinolution/math/physic/vec/model.hpp>
#include <variant>

struct MovePreset {
		Vec3 new_coord;
};

using AllPresets = std::variant<MovePreset>;