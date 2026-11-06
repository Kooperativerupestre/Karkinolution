#include "karkinolution/terrain/entity_map.hpp"

#include "karkinolution/organism/entities/entities.hpp"

#include <cassert>

namespace Karkinolution::Terrain {

	namespace {
		using AABB     = Karkinolution::Functional::Quadtree::AABB;
		using EntityId = Karkinolution::Organisms::Entities::EntityId;
		using Size     = Karkinolution::Math::Units::Size;
		using Vec2     = Karkinolution::Math::Vec::Vec2;

		AABB to_aabb(const Size &size, const Vec2 &position) {
			const Vec2 half_size{size.width.value / 2.0, size.height.value / 2.0};
			return {.min = position - half_size, .max = position + half_size};
		}

		AABB territory_aabb(const Size &size) {
			return to_aabb(size, Vec2{0.0, 0.0});
		}
	} // namespace

	EntityMap::EntityMap(const Size &size)
		: quadtree(territory_aabb(size)) {}

	bool EntityMap::add(EntityId id, const Vec2 &position, const Size &size, const Territory &territory) {
		if (quadtree.exists(id)) {
			return false;
		}
		const auto entity_aabb = to_aabb(size, position);
		if (!territory_aabb(territory.size()).contains(entity_aabb)) {
			return false;
		}
		return quadtree.add(id, entity_aabb);
	}

	bool EntityMap::remove(EntityId id, const Size &old_size, const Vec2 &old_position) {
		if (!quadtree.exists(id)) {
			return false;
		}
		const bool removed = quadtree.remove(id, to_aabb(old_size, old_position));
		assert(removed);
		return removed;
	}

	bool EntityMap::remove(EntityId id) {
		if (!quadtree.exists(id)) {
			return false;
		}
		const bool removed = quadtree.remove(id);
		assert(removed);
		return removed;
	}

	std::vector<EntityId> EntityMap::find(const Size &size, const Vec2 &position) const {
		return quadtree.find(to_aabb(size, position));
	}

	bool EntityMap::has_entity(EntityId id) const {
		return quadtree.exists(id);
	}

	std::optional<EntityId> EntityMap::if_find_one_stop(const Size &size, const Vec2 &position) const {
		return quadtree.if_find_one_stop(to_aabb(size, position));
	}

	bool EntityMap::update_coord(EntityId         id,
								 const Size      &current_size,
								 const Vec2      &current_position,
								 const Vec2      &new_position,
								 const Territory &territory) {
		if (!quadtree.exists(id)) {
			return false;
		}
		const auto old_bound = to_aabb(current_size, current_position);
		const auto new_bound = to_aabb(current_size, new_position);
		if (!territory_aabb(territory.size()).contains(new_bound)) {
			return false;
		}
		return quadtree.update(id, new_bound, old_bound) == Functional::Quadtree::UpdateOutput::UPDATED;
	}

	bool EntityMap::can_contain(const Size &size, const Vec2 &position) const {
		return quadtree.root().bounds().contains(to_aabb(size, position));
	}

} // namespace Karkinolution::Terrain
