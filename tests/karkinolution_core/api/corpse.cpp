#include "karkinolution/api/corpse.hpp"

#include "karkinolution/organism/entities/corpse/physiology.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/test/utils.hpp"

#include <gtest/gtest.h>

using Karkinolution::API::Corpse::erase;
using Karkinolution::API::Corpse::see_corpse;
using Karkinolution::API::Corpse::tick;
using Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint;
using Karkinolution::World::World;
using KShared::Core::Id::gen_id;

namespace CreatureConstructor = Karkinolution::Organisms::Entities::Creature::Constructor::CreatureConstructor;
namespace CorpsePhysiology    = Karkinolution::Organisms::Entities::Corpse::Physiology;

TEST(CorpseAPI, ReturnsEmptyReferencesForUnknownCorpses) {
	const World world = TestUtils::make_small_world();

	EXPECT_EQ(see_corpse(world, gen_id()), nullptr);
}

TEST(CorpseAPI, TicksAndReturnsUpdatedCorpseThroughOptionalReference) {
	World      world            = TestUtils::make_small_world();
	auto       creature         = CreatureConstructor::from_blueprint(CreatureBlueprint{});
	auto       corpse           = CorpsePhysiology::generate_corpse(creature);
	const auto corpse_id        = corpse.id;
	const auto initial_raw_meat = corpse.raw_meat.value;
	ASSERT_TRUE(world.add(std::move(corpse)));

	const auto output = tick(world, corpse_id);

	ASSERT_TRUE(output);
	EXPECT_EQ(output->id, corpse_id);
	EXPECT_FLOAT_EQ(output->raw_meat.value, initial_raw_meat * 0.1f);
}

TEST(CorpseAPI, EraseReturnsWhetherCorpseWasRemoved) {
	World      world     = TestUtils::make_small_world();
	auto       creature  = CreatureConstructor::from_blueprint(CreatureBlueprint{});
	auto       corpse    = CorpsePhysiology::generate_corpse(creature);
	const auto corpse_id = corpse.id;
	ASSERT_TRUE(world.add(std::move(corpse)));

	EXPECT_TRUE(erase(world, corpse_id));
	EXPECT_FALSE(erase(world, corpse_id));
	EXPECT_EQ(see_corpse(world, corpse_id), nullptr);
}
