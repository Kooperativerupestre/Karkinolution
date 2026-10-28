#include <gtest/gtest.h>
#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/frames/processor.hpp>
#include <karkinolution/binary/interpreters/corpse.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/binary/interpreters/soil.hpp>
#include <karkinolution/test/utils.hpp>

namespace {

	template <typename Payload>
	RequestFrame make_see_request(Resource resource, const Payload &payload) {
		return Karkinolution::Binary::Frames::Motor::make_request(
			RequestSubTypes::SEE,
			RequestId{1},
			resource,
			payload,
			Karkinolution::Binary::Frames::Models::Request::SeeExtra{
				.target = Karkinolution::Binary::Protocol::Request::SeeTarget::ONE});
	}

} // namespace

TEST(NonexistentResource, CreatureReturnsNone) {
	auto world = TestUtils::make_small_world();

	const auto payload  = CreatureSRI::Request::make_see_creature(0);
	const auto request  = make_see_request(Resource::CREATURE, payload);
	const auto response = Karkinolution::Binary::Frames::Processor::process(request, world);

	EXPECT_TRUE(Karkinolution::Binary::Frames::Analyzer::is_none(response));
}

TEST(NonexistentResource, CorpseReturnsNone) {
	auto world = TestUtils::make_small_world();

	const auto payload  = CorpseSRI::Request::make_see_corpse(0);
	const auto request  = make_see_request(Resource::CORPSE, payload);
	const auto response = Karkinolution::Binary::Frames::Processor::process(request, world);

	EXPECT_TRUE(Karkinolution::Binary::Frames::Analyzer::is_none(response));
}

TEST(NonexistentResource, SoilReturnsNone) {
	auto world = TestUtils::make_small_world();

	const auto payload  = SoilSRI::Request::make_see_soil(0);
	const auto request  = make_see_request(Resource::SOIL, payload);
	const auto response = Karkinolution::Binary::Frames::Processor::process(request, world);

	EXPECT_TRUE(Karkinolution::Binary::Frames::Analyzer::is_none(response));
}
