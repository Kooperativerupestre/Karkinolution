#include "karkinolution/terrain/soil/soil.hpp"

#include "karkinolution/terrain/soil/motor.hpp"
#include "shared/functional/data_structures.hpp"

#include <karkinolution/api/soil.hpp>

Karkinolution::API::Soil::SoilReference Karkinolution::API::Soil::see_soil(const Karkinolution::World::World &world,
																		   KShared::Core::Id::BaseIdType      id) {
	const auto soil =
		KShared::Functional::try_at(world.see_territory().registry(), Karkinolution::Terrain::Soil::SoilPieceId{id});

	if (soil == nullptr) {
		return nullptr;
	}
	return *soil;
}

std::vector<KShared::Core::Id::BaseIdType>
Karkinolution::API::Soil::see_all_soil_ids(const Karkinolution::World::World &world) {
	std::vector<KShared::Core::Id::BaseIdType> ids;
	ids.reserve(world.see_territory().registry().size());
	for (const auto &[id, soil] : world.see_territory().registry()) {
		static_cast<void>(soil);
		ids.push_back(id);
	}
	return ids;
}

Karkinolution::API::Soil::SoilReference Karkinolution::API::Soil::tick(Karkinolution::World::World  &world,
																	   KShared::Core::Id::BaseIdType id) {
	auto* soil =
		KShared::Functional::try_at(world.see_territory().registry(), Karkinolution::Terrain::Soil::SoilPieceId{id});

	if (soil == nullptr) {
		return nullptr;
	}

	Karkinolution::Terrain::Soil::Motor::SoilPieceMotor::run(*soil);
	return see_soil(world, id);
}

bool Karkinolution::API::Soil::erase(Karkinolution::World::World &world, KShared::Core::Id::BaseIdType id) {
	return world.see_territory().remove(Karkinolution::Terrain::Soil::SoilPieceId{id});
}
