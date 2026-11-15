#pragma once

#include "shared/core/id.hpp"

#include <karkinolution/terrain/soil/soil.hpp>
#include <karkinolution/world/world.hpp>
#include <shared/functional/optional_reference.hpp>

namespace Karkinolution {
	namespace API {
		namespace Soil {
			using SoilReference = KShared::Functional::OptionalReference<const Karkinolution::Terrain::Soil::SoilPiece>;

			SoilReference see_soil(const Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);
			SoilReference tick(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);

			inline const auto &see_all_soils(const Karkinolution::World::World &world) {
				return world.see_territory().registry();
			}
		} // namespace Soil
	} // namespace API
} // namespace Karkinolution
