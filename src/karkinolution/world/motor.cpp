#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/terrain/entity_map.hpp"

#include <karkinolution/world/motor.hpp>
#include <karkinolution/world/world.hpp>

void WorldMotor::remove(EntityId id, World &world) {
	world.organism_registry.entities.del(id);
	world.entity_map.root().remove(id);
}

bool WorldMotor::add(Entity &&entity, World &world) {
	const auto id                  = EntityGetters::get_id(entity);
	const auto was_inserted_on_map = EntityMapMotor::add(id,
														 EntityGetters::get_position(entity),
														 EntityGetters::get_size(entity),
														 world.entity_map,
														 world.territory);

	if (!was_inserted_on_map) {
		return false;
	}
	const auto was_insrted_on_registry =
		world.organism_registry.entities.try_add(id, std::move(entity));

	if (!was_insrted_on_registry) {
		for (size_t i = 0; i < 5; i++) {
			if (!EntityMapMotor::remove(id, world.entity_map)) {
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
