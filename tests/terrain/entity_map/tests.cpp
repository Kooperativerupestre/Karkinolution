#include "karkinolution/core/id.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/corpse/corpse.hpp>
#include <karkinolution/terrain/entity_map.hpp>
#include <karkinolution/terrain/terrain.hpp>

using Karkinolution::Core::Id::BaseIdType;
using Karkinolution::Functional::Quadtree::AABB;
using Karkinolution::Math::Units::Size;
using Karkinolution::Math::Vec::Vec2;
using Karkinolution::Organisms::Entities::create_corpse_id;
using Karkinolution::Organisms::Entities::EntityId;
using Karkinolution::Organisms::Entities::Corpse::Corpse;
using Karkinolution::Terrain::EntityMap;
using Karkinolution::Terrain::Territory;

namespace {

	Size entity_size() {
		return {10.0, 10.0};
	}

	EntityId corpse_id(BaseIdType id) {
		return create_corpse_id(id);
	}

	bool contains_id(const std::vector<EntityId> &ids, EntityId id) {
		return std::find(ids.begin(), ids.end(), id) != ids.end();
	}

} // namespace

TEST(EntityMapTest, AddStoresEntityAndIndexAtomically) {
	const Size map_size{100.0, 100.0};
	Territory  territory{map_size};
	EntityMap  entity_map{map_size};

	Corpse corpse{Size{.width = 10.0, .height = 10.0}, Vec2{0.0, 0.0}, Karkinolution::Organisms::RawMeat{3.0f}, 1};

	ASSERT_TRUE(entity_map.add(corpse.build_id(), corpse.position, corpse.size, territory));
	EXPECT_TRUE(entity_map.has_entity(corpse.build_id()));


	EXPECT_FALSE(entity_map.add(corpse.build_id(), corpse.position, corpse.size, territory));
	EXPECT_TRUE(entity_map.has_entity(corpse.build_id()));
}

TEST(EntityMapTest, AddRejectsEntityOutsideTerritory) {
	const Size map_size{5.0, 5.0};
	Territory  territory{map_size};
	EntityMap  entity_map{map_size};

	Corpse corpse{Size{.width = 1035.0, .height = 10.0}, Vec2{0.0, 530.0}, Karkinolution::Organisms::RawMeat{3.0f}, 1};

	EXPECT_FALSE(entity_map.add(corpse.build_id(), corpse.position, corpse.size, territory));
	EXPECT_FALSE(entity_map.has_entity(corpse.build_id()));
}

TEST(EntityMapTest, RemoveDeletesEntityAndIndex) {
	const Size map_size{500.0, 500.0};
	Territory  territory{map_size};
	EntityMap  entity_map{map_size};

	const auto id = corpse_id(3);
	const Size size{10.0, 10.0};
	const Vec2 position{0.0, 0.0};

	ASSERT_TRUE(entity_map.add(id, position, size, territory));
	EXPECT_TRUE(entity_map.remove(id, size, position));
	EXPECT_FALSE(entity_map.has_entity(id));
	EXPECT_FALSE(entity_map.remove(id, size, position));
}

TEST(EntityMapTest, FindReturnsEntitiesIntersectingRadius) {
	const Size map_size{20.0, 20.0};
	Territory  territory{map_size};
	EntityMap  entity_map{map_size};


	ASSERT_TRUE(entity_map.add(corpse_id(2), Vec2{0.0, 0.0}, Size{1.0, 1.0}, territory));
	ASSERT_TRUE(entity_map.add(corpse_id(22), Vec2{8.0, 4.0}, Size{0.0, 0.0}, territory));


	auto found = entity_map.find(Size{12.0, 12.0}, Vec2{0.0, 0.0});
	EXPECT_TRUE(contains_id(found, corpse_id(2)));
	EXPECT_FALSE(contains_id(found, corpse_id(22)));

	found = entity_map.find(Size{10.0, 10.0}, Vec2{8.0, 4.0});
	EXPECT_TRUE(contains_id(found, corpse_id(22)));
	EXPECT_FALSE(contains_id(found, corpse_id(2)));
}

TEST(EntityMapTest, UpdateCoordMovesEntityAndIndex) {
	const Size map_size{50.0, 50.0};
	Territory  territory{map_size};
	EntityMap  entity_map{map_size};
	const auto id               = corpse_id(8);
	const Vec2 initial_position = {0.0, 0.0};
	const Vec2 new_position     = {20.0, 0.0};

	ASSERT_TRUE(entity_map.add(id, initial_position, entity_size(), territory));
	ASSERT_TRUE(entity_map.update_coord(id, entity_size(), initial_position, new_position, territory));

	EXPECT_TRUE(contains_id(entity_map.find(Size{12.0, 12.0}, new_position), id));
	EXPECT_FALSE(contains_id(entity_map.find(Size{12.0, 12.0}, Vec2{-20.0, 0.0}), id));
}

TEST(EntityMapTest, UpdateCoordOutsideTerritoryIsAtomic) {
	const Size map_size{50.0, 50.0};
	Territory  territory{map_size};
	EntityMap  entity_map{map_size};
	const auto id           = corpse_id(9);
	const Vec2 old_position = {0.0, 0.0};

	ASSERT_TRUE(entity_map.add(id, old_position, entity_size(), territory));
	EXPECT_FALSE(entity_map.update_coord(id, entity_size(), old_position, Vec2{500.0, 0.0}, territory));

	EXPECT_TRUE(contains_id(entity_map.find(Size{12.0, 12.0}, old_position), id));
}

TEST(EntityMapTest, TerritoryBoundsAreCenteredAtOrigin) {
	// A territory of size S spans S/2 in each direction from the origin.
	// e.g. size {10, 10, 10} → bounds [-5, -5, -5] to [5, 5, 5].
	const Size size{10.0, 10.0};
	const AABB aabb{.min = Vec2{-size.width.value / 2.0, -size.height.value / 2.0},
					.max = Vec2{size.width.value / 2.0, size.height.value / 2.0}};

	EXPECT_DOUBLE_EQ(aabb.min.x, -5.0);
	EXPECT_DOUBLE_EQ(aabb.min.y, -5.0);
	EXPECT_DOUBLE_EQ(aabb.max.x, 5.0);
	EXPECT_DOUBLE_EQ(aabb.max.y, 5.0);
}
