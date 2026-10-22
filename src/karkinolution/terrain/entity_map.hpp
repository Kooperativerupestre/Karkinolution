#pragma once
#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/terrain/terrain.hpp"

#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/math/geometry/models.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/organism/reproduction/oviparous/egg.hpp>
#include <karkinolution/terrain/octree/aabb.hpp>
#include <karkinolution/terrain/octree/octree.hpp>
using EntityMap = Octree;

namespace EntityMapMotor {
	bool add(EntityId         id,
			 const Vec3      &position,
			 const Size      &size,
			 EntityMap       &map,
			 const Territory &territory);

	bool remove(EntityId id, EntityMap &map, const AABB &old_aabb);
	bool remove(EntityId id, EntityMap &map);

	std::vector<EntityId>
	find(const GeometryForms::Radius &radius, const Vec3 &position, const EntityMap &map);

	bool update_coord(EntityId         id,
					  EntityMap       &map,
					  const Size      &current_size,
					  const Vec3      &current_coord,
					  const Vec3      &new_coord,
					  const Territory &territory);
} // namespace EntityMapMotor
