#include "karkinolution/networking/frame/world/motor.hpp"
#include "karkinolution/networking/server_side/connection/protocol_sessions/world.hpp"
#include "karkinolution/world/world.hpp"

#include <gtest/gtest.h>
#include <karkinolution/networking/frame/world/interpreters/creature.hpp>
#include <karkinolution/testing/helpers/frame.hpp>
#include <karkinolution/testing/helpers/world.hpp>

using Karkinolution::Networking::ServerSide::Connection::ProtocolSessions::WorldSession;

TEST(ProtocolSessionTest, OneCompleteFrameProducesExactlyOneResponse) {
	auto         world = Karkinolution::Testing::World::make_small_world();
	WorldSession session{world};

	const auto frame = Karkinolution::Testing::Frame::make_see_creature_frame(7, 45);
	session.receive(frame);

	ASSERT_TRUE(session.has_response());
	const auto response = session.pop_response();
	EXPECT_EQ(response.get_request_id(), 7);
	EXPECT_FALSE(session.has_response());
}

TEST(ProtocolSessionTest, FrameSplitAcrossTwoReceivesProducesOneResponse) {
	auto         world = Karkinolution::Testing::World::make_small_world();
	WorldSession session{world};

	const auto frame = Karkinolution::Testing::Frame::make_see_creature_frame(8, 45);
	const auto half  = frame.size() / 2;
	std::span  all{frame};

	session.receive(all.first(half));
	EXPECT_FALSE(session.has_response());

	session.receive(all.subspan(half));
	ASSERT_TRUE(session.has_response());
	EXPECT_EQ(session.pop_response().get_request_id(), 8);
}
