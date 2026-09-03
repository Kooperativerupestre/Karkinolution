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

namespace AABBConversion {
	AABB to_aabb(const Size &size, const Vec3 &position);
	AABB to_aabb(const GeometryForms::Radius &radius, const Vec3 &position);
} // namespace AABBConversion

namespace EntityMapMotor {
	bool
	add(Entity &&entity, OrganismRegistry &registry, EntityMap &map, const Territory &territory);

	bool remove(Id id, OrganismRegistry &registry, EntityMap &map, const AABB &old_aabb);
	bool remove(Id id, OrganismRegistry &registry, EntityMap &map);

	std::vector<Id>
	find(const GeometryForms::Radius &radius, const Vec3 &position, const EntityMap &map);

	bool update_coord(Id                id,
					  OrganismRegistry &registry,
					  EntityMap        &map,
					  const Vec3       &new_coord,
					  const Territory  &territory);
} // namespace EntityMapMotor