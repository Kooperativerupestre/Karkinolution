#pragma once
#include "karkinolution/terrain/rtree/service.hpp"

#include <karkinolution/functional/base_storage.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil/soil.hpp>

namespace Karkinolution {
	namespace Terrain {
		class Territory : public Trees::RSTService<Soil::SoilPieceId,
												   Soil::SoilPiece,
												   Soil::SoilPieceRegistry> {
				using RSTService<Soil::SoilPieceId, Soil::SoilPiece, Soil::SoilPieceRegistry>::
					RSTService;
		};

	} // namespace Terrain
} // namespace Karkinolution
