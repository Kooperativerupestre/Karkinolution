#pragma once

#include "shared/core/id.hpp"

#include <karkinolution/terrain/soil/soil.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace API {
		namespace Soil {
			const Karkinolution::Terrain::Soil::SoilPiece* see_soil(const Karkinolution::World::World &world,
																	KShared::Core::Id::BaseIdType      id);

			inline const auto &see_all_soils(const Karkinolution::World::World &world) {
				return world.see_territory().registry();
			}
		} // namespace Soil
	} // namespace API
} // namespace Karkinolution