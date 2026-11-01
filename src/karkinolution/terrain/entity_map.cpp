#include "karkinolution/terrain/entity_map.hpp"

#include "karkinolution/core/id.hpp"
#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/organism/entities/entities.hpp"
#include "karkinolution/organism/entities/identity.hpp"

#include <cassert>
#include <karkinolution/functional/base_storage.hpp>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/organism/entities/egg/egg.hpp>
#include <karkinolution/terrain/octree/aabb.hpp>
#include <karkinolution/terrain/octree/octree.hpp>
#include <karkinolution/terrain/terrain.hpp>

namespace Karkinolution::Terrain {

bool EntityMap::add(Karkinolution::Organisms::Entities::EntityId         id,
					const Karkinolution::Math::Vec::Vec3      &position,
					const Karkinolution::Math::Units::Size      &size,
					const Karkinolution::Terrain::Territory &territory) {
	if (octree.root().exists(id)) {
		return false;
	}

	const auto entity_aabb =
		Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(size, position);

	if (!Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(
			 territory.size(), Karkinolution::Math::Vec::Vec3{0.0, 0.0, 0.0})
			 .contains(entity_aabb)) {
		return false;
	}

	octree.root().insert(Karkinolution::Terrain::Trees::Octree::OctreeEntry{
		.entity_id = id, .bound = entity_aabb});
	return true;
}

bool EntityMap::remove(Karkinolution::Organisms::Entities::EntityId id, const Karkinolution::Math::Units::Size &old_size, const Karkinolution::Math::Vec::Vec3 &old_position) {
	bool exists = octree.root().exists(id);

	if (!exists) {
		return false;
	}

	const auto old_aabb =
		Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(old_size, old_position);
	auto       was_removed = octree.root().remove(id, old_aabb);
	assert(was_removed);
	return true;
}

bool EntityMap::remove(Karkinolution::Organisms::Entities::EntityId id) {
	bool exists = octree.root().exists(id);

	if (!exists) {
		return false;
	}

	auto was_removed = octree.root().remove(id);
	assert(was_removed);
	return true;
}

std::vector<Karkinolution::Organisms::Entities::EntityId> EntityMap::find(const Karkinolution::Math::Units::Size &size,
									  const Karkinolution::Math::Vec::Vec3                             &position) const {
	auto aabb = Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(size, position);
	return octree.root().find_ids(aabb);
}

bool EntityMap::has_entity(Karkinolution::Organisms::Entities::EntityId id) const {

	return octree.root().exists(id);
}

std::optional<Karkinolution::Organisms::Entities::EntityId> EntityMap::if_find_one_stop(const Karkinolution::Math::Units::Size &size,
													const Karkinolution::Math::Vec::Vec3 &position) const {
	return octree.root().if_find_one_stop(
		Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(size, position));
}

bool EntityMap::update_coord(Karkinolution::Organisms::Entities::EntityId         id,
							 const Karkinolution::Math::Units::Size      &current_size,
							 const Karkinolution::Math::Vec::Vec3      &current_coord,
							 const Karkinolution::Math::Vec::Vec3      &new_coord,
							 const Karkinolution::Terrain::Territory &territory) {
	if (!octree.root().exists(id)) {
		return false;
	}


	const Karkinolution::Terrain::Trees::Octree::AABB old_bound =
		Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(current_size, current_coord);
	const Karkinolution::Terrain::Trees::Octree::AABB new_bound =
		Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(current_size, new_coord);

	if (!Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(
			 territory.size(), Karkinolution::Math::Vec::Vec3{0.0, 0.0, 0.0})
			 .contains(new_bound)) {
		return false;
	}
	const auto was_updated = octree.root().update(id, old_bound, new_bound);
	if (!was_updated) {
		return false;
	}

	assert(was_updated);
	return true;
}

bool EntityMap::can_contain(const Karkinolution::Math::Units::Size &size,
							const Karkinolution::Math::Vec::Vec3                             &position) const {
	return octree.root().can_contain(
		Karkinolution::Terrain::Trees::Octree::AABBConversion::to_aabb(size, position));
}

} // namespace Karkinolution::Terrain