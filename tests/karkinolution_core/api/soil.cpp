#include "karkinolution/api/soil.hpp"

#include "karkinolution/testing/helpers/world.hpp"

#include <gtest/gtest.h>

using Karkinolution::API::Soil::erase;
using Karkinolution::API::Soil::see_soil;
using Karkinolution::API::Soil::tick;
using Karkinolution::World::World;
using KShared::Core::Id::gen_id;

TEST(SoilAPI, ReturnsEmptyReferencesForUnknownSoils) {
	const World world = Karkinolution::Testing::World::make_small_world();

	EXPECT_EQ(see_soil(world, gen_id()), nullptr);
}

TEST(SoilAPI, TicksAndReturnsUpdatedSoilThroughOptionalReference) {
	World      world   = Karkinolution::Testing::World::make_small_world();
	const auto soil_id = world.see_territory().registry().begin()->first;

	const auto output = tick(world, soil_id);

	ASSERT_TRUE(output);
	EXPECT_EQ(output->id, soil_id);
}

TEST(SoilAPI, EraseReturnsWhetherSoilWasRemoved) {
	World      world   = Karkinolution::Testing::World::make_small_world();
	const auto soil_id = world.see_territory().registry().begin()->first;

	EXPECT_TRUE(erase(world, soil_id));
	EXPECT_FALSE(erase(world, soil_id));
	EXPECT_EQ(see_soil(world, soil_id), nullptr);
}
