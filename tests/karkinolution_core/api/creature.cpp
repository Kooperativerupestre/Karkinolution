#include "karkinolution/api/creature.hpp"

#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/testing/helpers/world.hpp"

#include <gtest/gtest.h>

using Karkinolution::API::Creature::erase;
using Karkinolution::API::Creature::make_creature;
using Karkinolution::API::Creature::see_creature;
using Karkinolution::API::Creature::tick;
using Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint;
using Karkinolution::World::World;
using KShared::Core::Id::gen_id;

namespace CreatureConstructor = Karkinolution::Organisms::Entities::Creature::Constructor::CreatureConstructor;

TEST(CreatureAPI, ReturnsEmptyReferencesForUnknownCreatures) {
	const World world = Karkinolution::Testing::World::make_small_world();

	EXPECT_EQ(see_creature(world, gen_id()), nullptr);
}

TEST(CreatureAPI, CreatesAndTicksCreatureThroughOptionalReferences) {
	World      world       = Karkinolution::Testing::World::make_small_world();
	const auto creature_id = gen_id();
	auto       creature    = CreatureConstructor::from_blueprint(CreatureBlueprint{.id = creature_id});
	ASSERT_TRUE(world.add(std::move(creature)));

	const auto before_tick = see_creature(world, creature_id);
	ASSERT_TRUE(before_tick);
	const auto intent_time_before_tick = before_tick->brain.intent().time;
	const auto ticked                  = tick(world, creature_id);

	ASSERT_TRUE(ticked);
	EXPECT_EQ(ticked.get(), before_tick.get());
	EXPECT_GT(ticked->brain.intent().time, intent_time_before_tick);
}

TEST(CreatureAPI, EraseReturnsWhetherCreatureWasRemoved) {
	World      world       = Karkinolution::Testing::World::make_small_world();
	const auto creature_id = gen_id();
	auto       creature    = CreatureConstructor::from_blueprint(CreatureBlueprint{.id = creature_id});
	ASSERT_TRUE(world.add(std::move(creature)));

	EXPECT_TRUE(erase(world, creature_id));
	EXPECT_FALSE(erase(world, creature_id));
	EXPECT_EQ(see_creature(world, creature_id), nullptr);
}
