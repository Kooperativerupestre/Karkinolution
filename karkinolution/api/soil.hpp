#pragma once

#include "shared/core/id.hpp"

#include <karkinolution/terrain/soil/soil.hpp>
#include <karkinolution/world/world.hpp>
#include <shared/functional/optional_reference.hpp>
#include <vector>

namespace Karkinolution {
	namespace API {
		namespace Soil {
			using SoilReference = KShared::Functional::OptionalReference<const Karkinolution::Terrain::Soil::SoilPiece>;

			SoilReference see_soil(const Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);
			SoilReference tick(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);
			bool          erase(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id);

			std::vector<KShared::Core::Id::BaseIdType> see_all_soil_ids(const Karkinolution::World::World &world);
		} // namespace Soil
	} // namespace API
} // namespace Karkinolution
