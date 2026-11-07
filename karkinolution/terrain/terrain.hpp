#pragma once
#include "karkinolution/terrain/rtree/service.hpp"

#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil/soil.hpp>

namespace Karkinolution {
	namespace Terrain {
		class Territory : public Trees::RSTService<Soil::SoilPieceId, Soil::SoilPiece> {
			public:

				using RSTService<Soil::SoilPieceId, Soil::SoilPiece>::RSTService;
		};

	} // namespace Terrain
} // namespace Karkinolution
