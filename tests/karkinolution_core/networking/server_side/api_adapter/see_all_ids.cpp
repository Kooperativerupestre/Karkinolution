#include "karkinolution/api/creature.hpp"
#include "karkinolution/networking/frame/world/interpreters/corpse.hpp"
#include "karkinolution/networking/frame/world/interpreters/creature.hpp"
#include "karkinolution/networking/frame/world/interpreters/soil.hpp"
#include "karkinolution/networking/frame/world/processor.hpp"
#include "karkinolution/organism/entities/corpse/physiology.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/testing/helpers/world.hpp"

#include <gtest/gtest.h>
#include <karkinolution/networking/frame/world/analyzer.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <shared/binary/deserializer.hpp>
#include <shared/binary/types/list.hpp>
#include <unordered_set>

namespace {
	using Karkinolution::Networking::Frames::World::RequestSubTypes;
	using Karkinolution::Networking::Frames::World::ResponseCardinality;
	using Karkinolution::Networking::Frames::World::ResponseSubTypes;
	using Karkinolution::Networking::Frames::World::Models::Request::SeeExtra;
	using Karkinolution::Networking::Frames::World::Request::Resource;
	using Karkinolution::Networking::Frames::World::Request::SeeTarget;
	using Karkinolution::World::World;
	using KShared::Core::Id::BaseIdType;
} // namespace

TEST(SeeAllIds, CreatureResponseContainsOnlyCreatureIds) {
	World      world    = Karkinolution::Testing::World::make_small_world();
	const auto creature = Karkinolution::API::Creature::make_creature(world, {}, {}, {}, {});
	ASSERT_TRUE(creature);

	const auto request =
		Karkinolution::Networking::Frames::World::Motor::make_request(RequestSubTypes::SEE,
																	  1,
																	  Resource::CREATURE,
																	  std::vector<std::byte>{},
																	  SeeExtra{.target = SeeTarget::ALL});
	const auto response = Karkinolution::Networking::Frames::World::Processor::process(request, world);

	ASSERT_EQ(response.get_sub_type(), ResponseSubTypes::CREATURE);
	ASSERT_EQ(response.get_cardinality(), ResponseCardinality::ALL);
	const auto ids = ListTDSI::deserialize<sizeof(BaseIdType)>(
		response.get_payload(),
		[](const std::vector<std::byte> &bytes, std::size_t offset) {
			return KShared::Binary::Deserializer::deserialize<BaseIdType>(bytes, offset);
		},
		0);
	ASSERT_EQ(ids, std::vector<BaseIdType>{creature->ontology.id});
}

TEST(SeeAllIds, CorpseResponseContainsOnlyCorpseIds) {
	World world    = Karkinolution::Testing::World::make_small_world();
	auto  creature = Karkinolution::Organisms::Entities::Creature::Constructor::CreatureConstructor::from_blueprint();
	auto  corpse   = Karkinolution::Organisms::Entities::Corpse::Physiology::generate_corpse(creature);
	const auto corpse_id = corpse.id;
	ASSERT_TRUE(world.add(std::move(corpse)));

	const auto request =
		Karkinolution::Networking::Frames::World::Motor::make_request(RequestSubTypes::SEE,
																	  1,
																	  Resource::CORPSE,
																	  std::vector<std::byte>{},
																	  SeeExtra{.target = SeeTarget::ALL});
	const auto response = Karkinolution::Networking::Frames::World::Processor::process(request, world);

	ASSERT_EQ(response.get_sub_type(), ResponseSubTypes::CORPSE);
	ASSERT_EQ(response.get_cardinality(), ResponseCardinality::ALL);
	const auto ids = ListTDSI::deserialize<sizeof(BaseIdType)>(
		response.get_payload(),
		[](const std::vector<std::byte> &bytes, std::size_t offset) {
			return KShared::Binary::Deserializer::deserialize<BaseIdType>(bytes, offset);
		},
		0);
	ASSERT_EQ(ids, std::vector<BaseIdType>{corpse_id});
}

TEST(SeeAllIds, SoilResponseContainsOnlySoilIds) {
	World                          world = Karkinolution::Testing::World::make_small_world();
	std::unordered_set<BaseIdType> expected_ids;
	for (const auto &[id, soil] : world.see_territory().registry()) {
		static_cast<void>(soil);
		expected_ids.insert(id);
	}

	const auto request =
		Karkinolution::Networking::Frames::World::Motor::make_request(RequestSubTypes::SEE,
																	  1,
																	  Resource::SOIL,
																	  std::vector<std::byte>{},
																	  SeeExtra{.target = SeeTarget::ALL});
	const auto response = Karkinolution::Networking::Frames::World::Processor::process(request, world);

	ASSERT_EQ(response.get_sub_type(), ResponseSubTypes::SOIL);
	ASSERT_EQ(response.get_cardinality(), ResponseCardinality::ALL);
	const auto ids = ListTDSI::deserialize<sizeof(BaseIdType)>(
		response.get_payload(),
		[](const std::vector<std::byte> &bytes, std::size_t offset) {
			return KShared::Binary::Deserializer::deserialize<BaseIdType>(bytes, offset);
		},
		0);
	EXPECT_EQ((std::unordered_set<BaseIdType>{ids.begin(), ids.end()}), expected_ids);
}
