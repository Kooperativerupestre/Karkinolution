#include <gtest/gtest.h>
#include <karkinolution/core/id_generator.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/terrain/terrain.hpp>
#include <karkinolution/terrain/terrain_gen.hpp>
#include <karkinolution/test/terrain_test_generator.hpp>

namespace {
	Territory give_me_territory(const Size &size = {10.0, 10.0, 10.0}) {
		return TerrainFactory::gen_terrain(size, 666, TerrainScale::NORMAL, 1.0, 10.0, 20.0);
	}
} // namespace

TEST(TerritoryTest, AddIsAtomic) {
	auto territory = give_me_territory();
	territory.clear();
	auto      &registry      = TerritoryTest::get_registry(territory);
	auto      &internal_tree = TerritoryTest::get_tree(territory);
	const auto common_id     = gen_id();
	// common id -> violating the unique id constraint of BaseStorage
	SoilPiece soil_A = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{0.0, 0.0, 0.0});
	soil_A.id        = common_id;

	SoilPiece soil_B = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{0.0, 0.0, 0.0});
	soil_B.id        = common_id;

	// unique id constraint
	const auto was_A_inserted = territory.add(std::move(soil_A));

	ASSERT_TRUE(was_A_inserted);

	const auto was_B_inserted = territory.add(std::move(soil_B));
	ASSERT_FALSE(was_B_inserted);


	ASSERT_TRUE(territory.internal_tree().exists(common_id));
	ASSERT_TRUE(territory.internal_tree().size() == 1);

	territory.clear();

	// if registry fails


	SoilPiece soil_C = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{0.0, 0.0, 0.0});
	soil_C.id        = common_id;

	SoilPiece soil_D = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{0.0, 0.0, 0.0});
	soil_D.id        = common_id;

	bool was_C_inserted_on_registry = registry.try_add(common_id, std::move(soil_C));
	ASSERT_TRUE(was_C_inserted_on_registry);

	bool was_D_inserted_on_territory = territory.add(std::move(soil_D));
	ASSERT_FALSE(was_D_inserted_on_territory);

	territory.clear();
	// if internal tree fails

	SoilPiece soil_E = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{0.0, 0.0, 0.0});
	soil_E.id        = common_id;

	SoilPiece soil_F = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{0.0, 0.0, 0.0});
	soil_F.id        = common_id;


	internal_tree.insert(common_id, BoxConversion::to_box(soil_E));
	const bool was_F_inserted_on_territory = territory.add(std::move(soil_F));
	ASSERT_FALSE(was_F_inserted_on_territory);
}

TEST(TerritoryTest, RemoveIsAtomic) {
	auto  territory     = give_me_territory();
	auto &registry      = TerritoryTest::get_registry(territory);
	auto &internal_tree = TerritoryTest::get_tree(territory);

	territory.clear();


	// if registry fails
	const auto common_id = gen_id();

	SoilPiece soil_A = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{0.0, 0.0, 0.0});
	soil_A.id        = common_id;
	internal_tree.insert(soil_A.id, BoxConversion::to_box(soil_A));

	const bool was_A_removed = territory.remove(common_id);

	ASSERT_FALSE(was_A_removed);
	ASSERT_TRUE(internal_tree.exists(common_id));
	ASSERT_FALSE(registry.exists(common_id));

	territory.clear();
	// if internal tree fails


	SoilPiece soil_B = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{0.0, 0.0, 0.0});
	soil_B.id        = common_id;

	registry.add(common_id, std::move(soil_B));

	const bool was_B_removed = territory.remove(common_id);

	ASSERT_FALSE(was_B_removed);
	ASSERT_TRUE(registry.exists(common_id));
	ASSERT_FALSE(internal_tree.exists(common_id));
}

TEST(TerritoryTest, AddRejectsPieceOutsideBoundary) {
	auto territory = give_me_territory();
	territory.clear();

	auto &registry      = TerritoryTest::get_registry(territory);
	auto &internal_tree = TerritoryTest::get_tree(territory);

	const auto id = gen_id();

	SoilPiece soil = SoilF::gen_soil_piece(SoilTypes::ROCK, 10, Vec3{100.0, 100.0, 100.0});
	soil.id        = id;

	const bool was_inserted = territory.add(std::move(soil));

	ASSERT_FALSE(was_inserted);
	ASSERT_FALSE(registry.exists(id));
	ASSERT_FALSE(internal_tree.exists(id));
	ASSERT_TRUE(registry.size() == 0);
	ASSERT_TRUE(internal_tree.size() == 0);
}

TEST(TerritoryTest, ClearReallyWorks) {
	auto territory = give_me_territory();

	territory.clear();

	ASSERT_TRUE(territory.internal_tree().get_all_ids().size() == 0);
	ASSERT_TRUE(territory.soils().size() == 0);
}
