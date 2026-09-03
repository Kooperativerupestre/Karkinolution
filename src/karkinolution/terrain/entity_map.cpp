#include "karkinolution/terrain/entity_map.hpp"
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

AABB AABBConversion::to_aabb(const Size& size, const Vec3& position) {
    const Vec3 half_size{size.lateral.value / 2.0, size.height.value / 2.0, size.back.value / 2.0};

    return AABB{.max = position + half_size, .min = position - half_size};
}

AABB to_abb(const GeometryForms::Radius& radius, const Vec3& position) {
    Vec3 extent{radius.value, radius.value, radius.value};

    return AABB{position - extent, position + extent};
}

bool EntityMapMotor::add(Entity&& entity, OrganismRegistry& registry, EntityMap& map,
                         const Territory& territory) {
    const auto size = EntityGetters::get_size(entity);
    const auto position = EntityGetters::get_position(entity);
    const auto entity_aabb = AABBConversion::to_aabb(size, position);

    if (!AABBConversion::to_aabb(territory.radius(), Vec3{0.0, 0.0, 0.0}).contains(entity_aabb)) {
        return false;
    }

    const auto id = EntityGetters::get_id(entity);

    map.root().insert(OctreeEntry{.entity_id = id, .bound = entity_aabb});

    bool was_inserted = registry.entities.try_add(id, std::move(std::move(entity)));

    if (!was_inserted) {
        map.root().remove(id, entity_aabb);
        return false;
    }
    return true;
}

bool EntityMapMotor::remove(Id id, OrganismRegistry& registry, EntityMap& map,
                            const AABB& old_aabb) {
    bool exists = registry.entities.exists(id) && map.root().exists(id);

    if (!exists) {
        return false;
    }
    registry.entities.del(id);
    auto was_removed = map.root().remove(id, old_aabb);
    assert(was_removed);
    return true;
}

bool EntityMapMotor::remove(Id id, OrganismRegistry& registry, EntityMap& map) {
    bool exists = registry.entities.exists(id) && map.root().exists(id);

    if (!exists) {
        return false;
    }

    registry.entities.del(id);
    auto was_removed = map.root().remove(id);
    assert(was_removed);
    return true;
}

std::vector<Id> EntityMapMotor::find(const GeometryForms::Radius& radius, const Vec3& position,
                                     const EntityMap& map) {
    auto aabb = AABBConversion::to_aabb(radius, position);
    return map.root().find_ids(aabb);
}

bool EntityMapMotor::update_coord(Id id, OrganismRegistry& registry, EntityMap& map,
                                  const Vec3& new_coord, const Territory& territory) {
    if (!map.root().exists(id)) {
        return false;
    }
    auto& entity = registry.entities.at(id);
    auto& position = EntityGetters::get_position(entity);
    const auto& size = EntityGetters::get_size(entity);
    const AABB old_bound = AABBConversion::to_aabb(size, position);

    position = new_coord;

    const AABB new_bound = AABBConversion::to_aabb(size, position);

    if (!AABBConversion::to_aabb(territory.radius(), Vec3{0.0, 0.0, 0.0}).contains(new_bound)) {
        return false;
    }
    const auto was_updated = map.root().update(id, old_bound, new_bound);
    assert(was_updated);
    return true;
}
