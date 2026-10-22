#include "karkinolution/terrain/entity_map.hpp"

#include "karkinolution/core/id.hpp"
#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/organism/entities/identity.hpp"

#include <cassert>
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/organism/reproduction/oviparous/egg.hpp>
#include <karkinolution/terrain/octree/aabb.hpp>
#include <karkinolution/terrain/octree/octree.hpp>
#include <karkinolution/terrain/terrain.hpp>

bool EntityMapMotor::add(EntityId         id,
						 const Vec3      &position,
						 const Size      &size,
						 EntityMap       &map,
						 const Territory &territory) {
	if (map.root().exists(id)) {
		return false;
	}

	const auto entity_aabb = AABBConversion::to_aabb(size, position);

	if (!AABBConversion::to_aabb(territory.size(), Vec3{0.0, 0.0, 0.0}).contains(entity_aabb)) {
		return false;
	}

	map.root().insert(OctreeEntry{.entity_id = id, .bound = entity_aabb});
	return true;
}

bool EntityMapMotor::remove(EntityId id, EntityMap &map, const AABB &old_aabb) {
	bool exists = map.root().exists(id);

	if (!exists) {
		return false;
	}
	auto was_removed = map.root().remove(id, old_aabb);
	assert(was_removed);
	return true;
}

bool EntityMapMotor::remove(EntityId id, EntityMap &map) {
	bool exists = map.root().exists(id);

	if (!exists) {
		return false;
	}

	auto was_removed = map.root().remove(id);
	assert(was_removed);
	return true;
}

std::vector<EntityId> EntityMapMotor::find(const GeometryForms::Radius &radius,
										   const Vec3                  &position,
										   const EntityMap             &map) {
	auto aabb = AABBConversion::to_aabb(radius, position);
	return map.root().find_ids(aabb);
}

bool EntityMapMotor::update_coord(EntityId         id,
								  EntityMap       &map,
								  const Size      &current_size,
								  const Vec3      &current_coord,
								  const Vec3      &new_coord,
								  const Territory &territory) {
	if (!map.root().exists(id)) {
		return false;
	}


	const AABB old_bound = AABBConversion::to_aabb(current_size, current_coord);
	const AABB new_bound = AABBConversion::to_aabb(current_size, new_coord);

	if (!AABBConversion::to_aabb(territory.size(), Vec3{0.0, 0.0, 0.0}).contains(new_bound)) {
		return false;
	}
	const auto was_updated = map.root().update(id, old_bound, new_bound);
	if (!was_updated) {
		return false;
	}

	assert(was_updated);
	return true;
}
