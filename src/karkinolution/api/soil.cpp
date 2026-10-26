#include "karkinolution/terrain/soil.hpp"

#include <karkinolution/api/soil.hpp>

const SoilPiece* Karkinolution::API::Soil::see_soil(const World &world, BaseIdType id) {
	const auto soil = world.territory.registry().try_at(SoilPieceId{id});

	if (soil == nullptr) {
		return nullptr;
	}
	return soil;
}