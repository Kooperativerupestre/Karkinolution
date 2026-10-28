
#include "karkinolution/networking/protocol_session.hpp"

#include "karkinolution/binary/frames/motor.hpp"
#include "karkinolution/world/world.hpp"

#include <gtest/gtest.h>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/test/utils.hpp>

TEST(ProtocolSessionTest, OneCompleteFrameProducesExactlyOneResponse) {
	auto            world = TestUtils::make_small_world();
	ProtocolSession session{world};

	const auto frame = TestUtils::make_see_creature_frame(7, 45);
	session.receive(frame);

	ASSERT_TRUE(session.has_response());
	const auto response = session.pop_response();
	EXPECT_EQ(response.get_request_id(), 7);
	EXPECT_FALSE(session.has_response());
}

TEST(ProtocolSessionTest, FrameSplitAcrossTwoReceivesProducesOneResponse) {
	auto            world = TestUtils::make_small_world();
	ProtocolSession session{world};

	const auto frame = TestUtils::make_see_creature_frame(8, 45);
	const auto half  = frame.size() / 2;
	std::span  all{frame};

	session.receive(all.first(half));
	EXPECT_FALSE(session.has_response());

	session.receive(all.subspan(half));
	ASSERT_TRUE(session.has_response());
	EXPECT_EQ(session.pop_response().get_request_id(), 8);
}
