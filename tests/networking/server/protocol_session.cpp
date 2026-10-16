// tests/networking/protocol_session_test.cpp
#include "karkinolution/networking/protocol_session.hpp"

#include "karkinolution/binary/frames/motor.hpp"
#include "karkinolution/binary/serialization/interpreters/creature/alias.hpp"
#include "karkinolution/world/world.hpp"

#include <gtest/gtest.h>

namespace {

	World make_small_world() {
		WorldPreset preset{
			.seed = 1,
			.size = Size{.lateral = Lateral{100.0}, .height = Height{100.0}, .depth = Depth{100.0}},
			.epsilon = 5.0f};
		return WorldFactory::create_world(preset);
	}

	std::vector<std::byte> make_see_creature_frame(Frames::Types::RequestId id,
												   BaseIdType               creature_id) {
		const auto payload = CreatureSRI::Request::make_see_creature(creature_id);
		return Frames::Motor::make_request_raw_bytes(
			RequestSubTypes::SEE,
			id,
			RequestResource::CREATURE,
			std::vector<std::byte>(payload.begin(), payload.end()));
	}

} // namespace

TEST(ProtocolSessionTest, OneCompleteFrameProducesExactlyOneResponse) {
	auto            world = make_small_world();
	ProtocolSession session{world};

	const auto frame = make_see_creature_frame(7, 45);
	session.receive(frame);

	ASSERT_TRUE(session.has_response());
	const auto response = session.pop_response();
	EXPECT_EQ(response.request_id, 7);
	EXPECT_FALSE(session.has_response());
}

TEST(ProtocolSessionTest, FrameSplitAcrossTwoReceivesProducesOneResponse) {
	auto            world = make_small_world();
	ProtocolSession session{world};

	const auto frame = make_see_creature_frame(8, 45);
	const auto half  = frame.size() / 2;
	std::span  all{frame};

	session.receive(all.first(half));
	EXPECT_FALSE(session.has_response());

	session.receive(all.subspan(half));
	ASSERT_TRUE(session.has_response());
	EXPECT_EQ(session.pop_response().request_id, 8);
}
