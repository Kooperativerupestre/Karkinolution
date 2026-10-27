#pragma once

#include "karkinolution/core/id.hpp"

#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace API {
		namespace Soil {
			const SoilPiece* see_soil(const World &world, BaseIdType id);

			inline auto see_all_soils(const World &world) {
				return world.territory.registry_values();
			}
		} // namespace Soil
	} // namespace API
} // namespace Karkinolution