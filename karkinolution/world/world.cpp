#include "karkinolution/world/world.hpp"

#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/organism/entities/identity.hpp"
#include "karkinolution/terrain/terrain.hpp"

#include <iostream>
#include <karkinolution/terrain/terrain_gen.hpp>
using namespace Karkinolution::World;

namespace Karkinolution::World {
	bool World::remove(Karkinolution::Organisms::Entities::EntityId id) {
		if (!exists_entity(id)) {
			return false;
		}
		entities_registry.remove(id);
		entity_map.remove(id);
		return false;
	}

	bool Karkinolution::World::World::add(Organisms::Entities::Entity &&entity) {
		const auto id                  = Organisms::Entities::EntityGetters::get_id(entity);
		const auto was_inserted_on_map = entity_map.add(id,
														Organisms::Entities::EntityGetters::get_position(entity),
														Organisms::Entities::EntityGetters::get_size(entity),
														territory);


		if (!was_inserted_on_map) {
			return false;
		}
		const auto was_insrted_on_registry = entities_registry.add(std::move(entity));

		if (!was_insrted_on_registry) {
			for (size_t i = 0; i < 5; i++) {
				if (!entity_map.remove(id)) {
					continue;
				}
				return false;
			}
			std::cout << "Couldn't remove entity. Entity with id {} exists in map, but it doesn't "
						 "exist in registry\n";
			return false;
		}

		return true;
	}

	bool Karkinolution::World::World::update(Karkinolution::Organisms::Entities::EntityId id,
											 const KShared::Math::Vec::Vec2              &new_position) {
		if (!exists_entity(id)) {
			return false;
		}

		auto &entity   = entities_registry.at(id);
		auto &position = Organisms::Entities::EntityGetters::get_position(entity);
		if (!entity_map.update_coord(id,
									 Organisms::Entities::EntityGetters::get_size(entity),
									 position,
									 new_position,
									 territory)) {
			return false;
		}
		position = new_position;
		return true;
	}

	World::World(const WorldPreset &preset)
		: entity_map(preset.size)
		, natures(preset.size)
		, territory(Terrain::TerrainFactory::gen_terrain(preset.size,
														 preset.seed,
														 Terrain::TerrainFactory::TerrainScale::NORMAL,
														 preset.epsilon,
														 KShared::Math::Geometry::Radius(10.0),
														 KShared::Math::Geometry::Radius(20.0))) {}
} // namespace Karkinolution::World
