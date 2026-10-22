#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/stats.hpp"
#include "karkinolution/world/world.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <karkinolution/organism/entities/entities.hpp>
#include <karkinolution/terrain/entity_map.hpp>
#include <karkinolution/terrain/terrain.hpp>
#include <karkinolution/world/motor.hpp>

namespace {

	Size entity_size() {
		return {10.0, 10.0, 10.0};
	}

	Territory territory() {
		return Territory{Size{100.0, 100.0, 100.0}};
	}

	World make_world() {
		return WorldFactory::create_world(
			WorldPreset{.seed = 2, .size = Size{50.0, 50.0, 50.0}, .epsilon = 20});
	}

	Entity corpse(BaseIdType id, const Vec3 &position) {
		return Corpse{entity_size(), position, RawMeat{100.0F}, id};
	}

	EntityId corpse_id(BaseIdType id) {
		return EntityIDF::create_corpse_id(id);
	}

	bool contains_id(const std::vector<EntityId> &ids, EntityId id) {
		return std::find(ids.begin(), ids.end(), id) != ids.end();
	}

} // namespace

TEST(EntityMapTest, AddStoresEntityAndIndexAtomically) {
	World world = WorldFactory::create_world(
		WorldPreset{.seed    = 2,
					.size    = Size{.lateral = 100.0, .height = 100.0, .depth = 100.0},
					.epsilon = 20});

	Corpse corpse{Size{.lateral = 10.0, .height = 10.0, .depth = 10.0},
				  Vec3{0.0, 0.0, 0.0},
				  RawMeat{3.0f},
				  1};


	ASSERT_TRUE(EntityMapMotor::add(corpse.build_id(),
									corpse.position,
									corpse.size,
									world.entity_map,
									world.territory));
	EXPECT_TRUE(world.entity_map.root().exists(corpse.build_id()));


	EXPECT_FALSE(EntityMapMotor::add(corpse.build_id(),
									 corpse.position,
									 corpse.size,
									 world.entity_map,
									 world.territory));
	EXPECT_TRUE(world.entity_map.root().exists(corpse.build_id()));
}

TEST(EntityMapTest, AddRejectsEntityOutsideTerritory) {
	auto world = WorldFactory::create_world(
		WorldPreset{.seed = 2, .size = Size{5.0, 5.0, 5.0}, .epsilon = 1});

	Corpse corpse{Size{.lateral = 1035.0, .height = 10.0, .depth = 10.0},
				  Vec3{0.0, 530.0, 0.0},
				  RawMeat{3.0f},
				  1};

	EXPECT_FALSE(EntityMapMotor::add(corpse.build_id(),
									 corpse.position,
									 corpse.size,
									 world.entity_map,
									 world.territory));
	EXPECT_FALSE(world.entity_map.root().exists(corpse.build_id()));
}

TEST(EntityMapTest, RemoveDeletesEntityAndIndex) {
	auto world = WorldFactory::create_world(
		WorldPreset{.seed = 2, .size = Size{500.0, 500.0, 500.0}, .epsilon = 100});

	const auto id = corpse_id(3);
	const Size size{10.0, 10.0, 10.0};
	const Vec3 position{0.0, 0.0, 0.0};
	const auto bound = AABBConversion::to_aabb(entity_size(), Vec3{0.0, 0.0, 0.0});


	ASSERT_TRUE(EntityMapMotor::add(id, position, size, world.entity_map, world.territory));
	EXPECT_TRUE(EntityMapMotor::remove(id, world.entity_map, bound));
	EXPECT_FALSE(world.entity_map.root().exists(id));
	EXPECT_FALSE(EntityMapMotor::remove(id, world.entity_map, bound));
}

TEST(EntityMapTest, FindReturnsEntitiesIntersectingRadius) {
	auto world = WorldFactory::create_world(
		WorldPreset{.seed = 1, .size = Size{20.0, 20.0, 20.0}, .epsilon = 4});


	ASSERT_TRUE(EntityMapMotor::add(corpse_id(2),
									Vec3{0.0, 0.0, 0.0},
									Size{1.0, 1.0, 1.0},
									world.entity_map,
									world.territory));
	ASSERT_TRUE(EntityMapMotor::add(corpse_id(22),
									Vec3{8.0, 4.0, 8.0},
									Size{0.0, 0.0, 0.0},
									world.entity_map,
									world.territory));


	auto found =
		EntityMapMotor::find(GeometryForms::Radius{6.0}, Vec3{0.0, 0.0, 0.0}, world.entity_map);
	EXPECT_TRUE(contains_id(found, corpse_id(2)));
	EXPECT_FALSE(contains_id(found, corpse_id(22)));

	found = EntityMapMotor::find(GeometryForms::Radius{5.0}, Vec3{8.0, 4.0, 8.0}, world.entity_map);
	EXPECT_TRUE(contains_id(found, corpse_id(22)));
	EXPECT_FALSE(contains_id(found, corpse_id(2)));
}

TEST(EntityMapTest, UpdateCoordMovesEntityAndIndex) {
	auto       world            = make_world();
	const auto id               = corpse_id(8);
	const Vec3 initial_position = {0.0, 0.0, 0.0};
	const Vec3 new_position     = {20.0, 0.0, 0.0};

	ASSERT_TRUE(EntityMapMotor::add(id,
									initial_position,
									entity_size(),
									world.entity_map,
									world.territory));
	ASSERT_TRUE(EntityMapMotor::update_coord(id,
											 world.entity_map,
											 entity_size(),
											 initial_position,
											 new_position,
											 world.territory));

	EXPECT_TRUE(contains_id(
		EntityMapMotor::find(GeometryForms::Radius{6.0}, new_position, world.entity_map),
		id));
	EXPECT_FALSE(contains_id(
		EntityMapMotor::find(GeometryForms::Radius{6.0}, Vec3{-20.0, 0.0, 0.0}, world.entity_map),
		id));
}

TEST(EntityMapTest, UpdateCoordOutsideTerritoryIsAtomic) {
	auto       world        = make_world();
	const auto id           = corpse_id(9);
	const Vec3 old_position = {0.0, 0.0, 0.0};

	ASSERT_TRUE(
		EntityMapMotor::add(id, old_position, entity_size(), world.entity_map, world.territory));
	EXPECT_FALSE(EntityMapMotor::update_coord(id,
											  world.entity_map,
											  entity_size(),
											  old_position,
											  Vec3{500.0, 0.0, 0.0},
											  world.territory));

	EXPECT_TRUE(contains_id(
		EntityMapMotor::find(GeometryForms::Radius{6.0}, old_position, world.entity_map),
		id));
}

TEST(EntityMapTest, TerritoryBoundsAreCenteredAtOrigin) {
	// A territory of size S spans S/2 in each direction from the origin.
	// e.g. size {10, 10, 10} → bounds [-5, -5, -5] to [5, 5, 5].
	const Size size{10.0, 10.0, 10.0};
	const AABB aabb = AABBConversion::to_aabb(size, Vec3{0.0, 0.0, 0.0});

	EXPECT_DOUBLE_EQ(aabb.min.x, -5.0);
	EXPECT_DOUBLE_EQ(aabb.min.y, -5.0);
	EXPECT_DOUBLE_EQ(aabb.min.z, -5.0);
	EXPECT_DOUBLE_EQ(aabb.max.x, 5.0);
	EXPECT_DOUBLE_EQ(aabb.max.y, 5.0);
	EXPECT_DOUBLE_EQ(aabb.max.z, 5.0);
}
