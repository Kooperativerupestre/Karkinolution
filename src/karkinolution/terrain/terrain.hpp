#pragma once
#include "karkinolution/terrain/rtree/service.hpp"

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/terrain/rtree/rtree.hpp>
#include <karkinolution/terrain/soil.hpp>

class Territory : public RSTService<SoilPieceId, SoilPiece, SoilPieceRegistry> {
		using RSTService<SoilPieceId, SoilPiece, SoilPieceRegistry>::RSTService;
};
