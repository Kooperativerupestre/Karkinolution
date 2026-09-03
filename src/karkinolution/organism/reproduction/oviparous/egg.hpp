#pragma once
#include "karkinolution/organism/entities/identity.hpp"

#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/foods/nutrient.hpp>
#include <karkinolution/organism/stats.hpp>
#include <vector>

using OrganismStats::Health::Health;
using PhysicsStats::Volume;

class ShellValue : RuntimeLimitedValue<ShellValue, float> {
		using RuntimeLimitedValue<ShellValue, float>::RuntimeLimitedValue;
};

struct Shell {
		NormalizedValue<float> resistance;
		ShellValue             state;
		Volume                 volume;
};

struct EggNutrients {
		Nutrient nutrient;
		Health   health;
};

struct Egg {
		uint64_t        id;
		Shell           shell;
		EggNutrients    nutrients;
		std::vector<Id> embryos;
		Vec3            position;
		Size            size;

		[[nodiscard]] Id build_id() const noexcept {
			return IDF::create_egg_id(id);
		}
};
