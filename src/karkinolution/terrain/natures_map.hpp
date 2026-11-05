#pragma once
#include "karkinolution/organism/nature/identity.hpp"
#include "karkinolution/terrain/rtree/service.hpp"

#include <karkinolution/organism/nature/natures.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>

namespace Karkinolution::Terrain {

	class NaturesMap
		: public Karkinolution::Terrain::Trees::RSTService<Organisms::Nature::NatureId,
														   Organisms::Nature::NatureEntity> {
			using Karkinolution::Terrain::Trees::RSTService<
				Karkinolution::Organisms::Nature::NatureId,
				Karkinolution::Organisms::Nature::NatureEntity>::RSTService;
	};

} // namespace Karkinolution::Terrain
