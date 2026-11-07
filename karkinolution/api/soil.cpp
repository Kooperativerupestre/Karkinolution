#include "karkinolution/terrain/soil/soil.hpp"

#include "shared/functional/data_structures.hpp"

#include <karkinolution/api/soil.hpp>

const Karkinolution::Terrain::Soil::SoilPiece*
Karkinolution::API::Soil::see_soil(const Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id) {
	const auto soil =
		KShared::Functional::try_at(world.see_territory().registry(), Karkinolution::Terrain::Soil::SoilPieceId{id});

	if (soil == nullptr) {
		return nullptr;
	}
	return soil;
}