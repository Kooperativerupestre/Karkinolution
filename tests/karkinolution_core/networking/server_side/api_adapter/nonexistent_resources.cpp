#include <gtest/gtest.h>
#include <karkinolution/networking/frame/world/analyzer.hpp>
#include <karkinolution/networking/frame/world/core_aliases.hpp>
#include <karkinolution/networking/frame/world/interpreters/corpse.hpp>
#include <karkinolution/networking/frame/world/interpreters/creature.hpp>
#include <karkinolution/networking/frame/world/interpreters/soil.hpp>
#include <karkinolution/networking/frame/world/message_interpreters/corpse.hpp>
#include <karkinolution/networking/frame/world/message_interpreters/creature.hpp>
#include <karkinolution/networking/frame/world/message_interpreters/soil.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <karkinolution/networking/frame/world/processor.hpp>
#include <karkinolution/testing/helpers/world.hpp>

namespace {
	using ::RequestFrame;
	using Karkinolution::Networking::Frames::World::RequestId;
	using Karkinolution::Networking::Frames::World::RequestSubTypes;
	using Karkinolution::Networking::Frames::World::Resource;

	template <typename Payload> RequestFrame make_see_request(Resource resource, const Payload &payload) {
		return Karkinolution::Networking::Frames::World::Motor::make_request(
			RequestSubTypes::SEE,
			RequestId{1},
			resource,
			payload,
			Karkinolution::Networking::Frames::World::Models::Request::SeeExtra{
				.target = Karkinolution::Networking::Frames::World::Request::SeeTarget::ONE});
	}

} // namespace

TEST(NonexistentResource, CreatureReturnsNone) {
	auto world = Karkinolution::Testing::World::make_small_world();

	const auto payload  = CreatureMessageSRI::make_see_creature(0);
	const auto request  = make_see_request(Resource::CREATURE, payload);
	const auto response = Karkinolution::Networking::Frames::World::Processor::process(request, world);

	EXPECT_TRUE(Karkinolution::Networking::Frames::World::Analyzer::is_none(response));
}

TEST(NonexistentResource, CorpseReturnsNone) {
	auto world = Karkinolution::Testing::World::make_small_world();

	const auto payload  = CorpseMessageSRI::make_see_corpse(0);
	const auto request  = make_see_request(Resource::CORPSE, payload);
	const auto response = Karkinolution::Networking::Frames::World::Processor::process(request, world);

	EXPECT_TRUE(Karkinolution::Networking::Frames::World::Analyzer::is_none(response));
}

TEST(NonexistentResource, SoilReturnsNone) {
	auto world = Karkinolution::Testing::World::make_small_world();

	const auto payload  = SoilMessageSRI::make_see_soil(0);
	const auto request  = make_see_request(Resource::SOIL, payload);
	const auto response = Karkinolution::Networking::Frames::World::Processor::process(request, world);

	EXPECT_TRUE(Karkinolution::Networking::Frames::World::Analyzer::is_none(response));
}
