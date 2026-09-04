#include <algorithm>
#include <gtest/gtest.h>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/organism/registry.hpp>
#include <karkinolution/terrain/entity_map.hpp>
#include <karkinolution/terrain/terrain.hpp>

namespace {

	Size entity_size() {
		return {10.0, 10.0, 10.0};
	}

	Territory territory() {
		return Territory{Size{100.0, 100.0, 100.0}};
	}

	Entity corpse(uint64_t id, const Vec3 &position) {
		return Corpse{entity_size(), position, RawMeat{100.0F}, id};
	}

	Id corpse_id(uint64_t id) {
		return IDF::create_corpse_id(id);
	}

	bool contains_id(const std::vector<Id> &ids, Id id) {
		return std::find(ids.begin(), ids.end(), id) != ids.end();
	}

} // namespace

TEST(EntityMapTest, AddStoresEntityAndIndexAtomically) {
	OrganismRegistry registry;
	EntityMap        map;
	auto             world = territory();

	const auto id = corpse_id(1);
	ASSERT_TRUE(EntityMapMotor::add(corpse(1, Vec3{0.0, 0.0, 0.0}), registry, map, world));
	EXPECT_TRUE(registry.entities.exists(id));
	EXPECT_TRUE(map.root().exists(id));
	EXPECT_EQ(registry.entities.size(), 1);

	EXPECT_FALSE(EntityMapMotor::add(corpse(1, Vec3{20.0, 0.0, 0.0}), registry, map, world));
	EXPECT_EQ(registry.entities.size(), 1);
	EXPECT_TRUE(map.root().exists(id));
}

TEST(EntityMapTest, AddRejectsEntityOutsideTerritory) {
	OrganismRegistry registry;
	EntityMap        map;
	auto             world = territory();
	const auto       id    = corpse_id(2);

	EXPECT_FALSE(EntityMapMotor::add(corpse(2, Vec3{50.0, 0.0, 0.0}), registry, map, world));
	EXPECT_FALSE(registry.entities.exists(id));
	EXPECT_FALSE(map.root().exists(id));
}

TEST(EntityMapTest, RemoveDeletesEntityAndIndex) {
	OrganismRegistry registry;
	EntityMap        map;
	auto             world = territory();
	const auto       id    = corpse_id(3);
	const auto       bound = AABBConversion::to_aabb(entity_size(), Vec3{0.0, 0.0, 0.0});

	ASSERT_TRUE(EntityMapMotor::add(corpse(3, Vec3{0.0, 0.0, 0.0}), registry, map, world));
	EXPECT_TRUE(EntityMapMotor::remove(id, registry, map, bound));
	EXPECT_FALSE(registry.entities.exists(id));
	EXPECT_FALSE(map.root().exists(id));
	EXPECT_FALSE(EntityMapMotor::remove(id, registry, map));
}

TEST(EntityMapTest, RemoveIsAtomicWhenEitherSideDoesNotContainEntity) {
	OrganismRegistry registry;
	EntityMap        map;
	const auto       id = corpse_id(4);

	registry.entities.add(id, corpse(4, Vec3{0.0, 0.0, 0.0}));
	EXPECT_FALSE(EntityMapMotor::remove(id, registry, map));
	EXPECT_TRUE(registry.entities.exists(id));

	registry.entities.clear();
	map.root().insert(
		OctreeEntry{.entity_id = id,
					.bound     = AABBConversion::to_aabb(entity_size(), Vec3{0.0, 0.0, 0.0})});
	EXPECT_FALSE(EntityMapMotor::remove(id, registry, map));
	EXPECT_TRUE(map.root().exists(id));
}

TEST(EntityMapTest, FindReturnsEntitiesIntersectingRadius) {
	OrganismRegistry registry;
	EntityMap        map;
	auto             world = territory();

	ASSERT_TRUE(EntityMapMotor::add(corpse(5, Vec3{-20.0, 0.0, 0.0}), registry, map, world));
	ASSERT_TRUE(EntityMapMotor::add(corpse(6, Vec3{0.0, 0.0, 0.0}), registry, map, world));
	ASSERT_TRUE(EntityMapMotor::add(corpse(7, Vec3{20.0, 0.0, 0.0}), registry, map, world));

	const auto found = EntityMapMotor::find(GeometryForms::Radius{6.0}, Vec3{0.0, 0.0, 0.0}, map);
	EXPECT_TRUE(contains_id(found, corpse_id(6)));
	EXPECT_FALSE(contains_id(found, corpse_id(5)));
	EXPECT_FALSE(contains_id(found, corpse_id(7)));
}

TEST(EntityMapTest, UpdateCoordMovesEntityAndIndex) {
	OrganismRegistry registry;
	EntityMap        map;
	auto             world = territory();
	const auto       id    = corpse_id(8);

	ASSERT_TRUE(EntityMapMotor::add(corpse(8, Vec3{-20.0, 0.0, 0.0}), registry, map, world));
	ASSERT_TRUE(EntityMapMotor::update_coord(id, registry, map, Vec3{20.0, 0.0, 0.0}, world));

	EXPECT_EQ(EntityGetters::get_position(registry.entities.at(id)), Vec3(20.0, 0.0, 0.0));
	EXPECT_TRUE(
		contains_id(EntityMapMotor::find(GeometryForms::Radius{6.0}, Vec3{20.0, 0.0, 0.0}, map),
					id));
	EXPECT_FALSE(
		contains_id(EntityMapMotor::find(GeometryForms::Radius{6.0}, Vec3{-20.0, 0.0, 0.0}, map),
					id));
}

TEST(EntityMapTest, UpdateCoordOutsideTerritoryIsAtomic) {
	OrganismRegistry registry;
	EntityMap        map;
	auto             world = territory();
	const auto       id    = corpse_id(9);
	const Vec3       old_position{0.0, 0.0, 0.0};

	ASSERT_TRUE(EntityMapMotor::add(corpse(9, old_position), registry, map, world));
	EXPECT_FALSE(EntityMapMotor::update_coord(id, registry, map, Vec3{50.0, 0.0, 0.0}, world));

	EXPECT_EQ(EntityGetters::get_position(registry.entities.at(id)), old_position);
	EXPECT_TRUE(
		contains_id(EntityMapMotor::find(GeometryForms::Radius{6.0}, old_position, map), id));
}
