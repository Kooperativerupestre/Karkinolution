#pragma once
#include "shared/core/id.hpp"
#include "shared/math/stats/runtime_values.hpp"

#include <shared/math/geometry/models.hpp>
#include <shared/math/physic/vec/model.hpp>
#include <shared/math/units.hpp>
#include <karkinolution/organism/nature/identity.hpp>
#include <karkinolution/organism/values.hpp>

namespace Karkinolution {
	namespace Organisms {
		namespace Nature {
			namespace Grass {
				class GrassMatter : public RuntimeLimitedValue<GrassMatter, float> {
						using RuntimeLimitedValue<GrassMatter, float>::RuntimeLimitedValue;
				};

				class Grass {
					public:

						BaseIdType id;

						Quality                quality;
						Vec2                   position;
						KShared::Math::Geometry::Radius radius;
						GrassMatter            matter;

						bool constexpr should_disappear() const {
							return matter.is_zero();
						}

						[[nodiscard]] NatureId constexpr build_id() const {
							return NatureId{.type = NatureTypes::GRASS, .value = id};
						}
				};

			} // namespace Grass
		} // namespace Nature
	} // namespace Organisms
} // namespace Karkinolution
