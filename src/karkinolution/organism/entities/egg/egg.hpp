#pragma once
#include "karkinolution/organism/entities/identity.hpp"

#include <shared/math/physic/vec/model.hpp>
#include <shared/math/stats/runtime_values.hpp>
#include <shared/math/units.hpp>
#include <karkinolution/organism/foods/nutrient.hpp>
#include <karkinolution/organism/internal_aliases.hpp>
#include <karkinolution/organism/values.hpp>
#include <vector>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Egg {
				class ShellValue : RuntimeLimitedValue<ShellValue, float> {
						using RuntimeLimitedValue<ShellValue, float>::RuntimeLimitedValue;
				};

				struct Shell {
						NormalizedValue<float> resistance;
						ShellValue             state;
						Area                   area;
				};

				struct EggNutrients {
						Nutrient nutrient;
						Health   health;
				};

				struct Egg {
						uint64_t              id;
						Shell                 shell;
						EggNutrients          nutrients;
						std::vector<EntityId> embryos;
						Vec2                  position;
						Size                  size;

						[[nodiscard]] EntityId build_id() const noexcept {
							return create_egg_id(id);
						}
				};

			} // namespace Egg
		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
