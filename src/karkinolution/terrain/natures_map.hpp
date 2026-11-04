#pragma once
#include "karkinolution/terrain/rtree/service.hpp"

#include <karkinolution/functional/base_storage.hpp>
#include <karkinolution/organism/nature/natures.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>

namespace Karkinolution::Terrain {

	class NaturesMap : public Karkinolution::Terrain::Trees::RSTService<
						   Karkinolution::Organisms::Nature::NatureId,
						   Karkinolution::Organisms::Nature::NatureEntity,
						   Karkinolution::Organisms::Nature::NatureRegistry> {
			using Karkinolution::Terrain::Trees::RSTService<
				Karkinolution::Organisms::Nature::NatureId,
				Karkinolution::Organisms::Nature::NatureEntity,
				Karkinolution::Organisms::Nature::NatureRegistry>::RSTService;
	};

} // namespace Karkinolution::Terrain
