#include "karkinolution/networking/server_side/connection/protocol_sessions/session_manager.hpp"

#include "karkinolution/networking/frame/system/interpreter.hpp"
#include "karkinolution/networking/frame/system/message_interpreters/system.hpp"
#include "karkinolution/networking/frame/system/motor.hpp"
#include "karkinolution/networking/scope/motor.hpp"
#include "karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/networking/server_side/simulation/simulation.hpp"
#include "karkinolution/testing/helpers/frame.hpp"
#include "karkinolution/testing/helpers/world.hpp"
#include "karkinolution/world/world_storage.hpp"

#include <gtest/gtest.h>
#include <variant>

namespace {
	using ::ResponseFrame;
	using Karkinolution::Networking::Frames::System::SystemCallSubTypes;
	using Karkinolution::Networking::Frames::System::SystemOutputSubTypes;
	using Karkinolution::Networking::Frames::System::Motor::make_raw_system_call;
	using Karkinolution::Networking::Scope::Motor::make_system_frame;
	using Karkinolution::Networking::Scope::Motor::make_world_frame;
	using Karkinolution::Networking::ServerSide::Connection::ProtocolSessions::SessionManager;
	using Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter;
	using Karkinolution::Networking::ServerSide::Simulation::Simulation;
	using Karkinolution::World::WorldStorage;

} // namespace

TEST(SessionManager, RoutesSystemFramesAndWorldFramesWithoutForwardingScope) {
	WorldStorage storage;
	ASSERT_TRUE(storage.add(Karkinolution::Testing::World::make_small_world()));
	const auto        world_id = storage.get_all_world_ids().front();
	Simulation        simulation{storage};
	SimulationAdapter adapter{simulation, 17};
	SessionManager    manager{adapter};

	const auto world_list_call =
		make_system_frame(make_raw_system_call(SystemCallSubTypes::GET_ALL_WORLD_IDS, 1, std::vector<std::byte>{}));
	manager.receive(world_list_call);
	ASSERT_TRUE(manager.has_response());
	const auto world_list_response =
		std::get<Karkinolution::Networking::Frames::System::Models::SystemOutput::SystemOutput>(manager.pop_response());
	EXPECT_EQ(world_list_response.get_call_id(), 1);
	EXPECT_EQ(world_list_response.get_sub_type(), SystemOutputSubTypes::AllWorldIds);

	const auto config_payload =
		Karkinolution::Networking::Frames::System::MessageInterpreters::SystemCall::SRI::make_config(world_id);
	manager.receive(make_system_frame(make_raw_system_call(SystemCallSubTypes::CONFIG, 2, config_payload)));
	ASSERT_TRUE(manager.has_response());
	const auto config_response =
		std::get<Karkinolution::Networking::Frames::System::Models::SystemOutput::SystemOutput>(manager.pop_response());
	EXPECT_EQ(config_response.get_sub_type(), SystemOutputSubTypes::ConfigOutput);
	EXPECT_TRUE(
		Karkinolution::Networking::Frames::System::Interpreter::DSI::get_config_output(config_response.get_payload())
			.could_connect);

	const auto world_request = make_world_frame(Karkinolution::Testing::Frame::make_see_creature_frame(3, 45));
	manager.receive(world_request);
	ASSERT_TRUE(manager.has_response());
	const auto world_response = std::get<ResponseFrame>(manager.pop_response());
	EXPECT_EQ(world_response.get_request_id(), 3);
}

TEST(SessionManager, RejectsWorldFramesBeforeWorldAcquisition) {
	WorldStorage storage;
	ASSERT_TRUE(storage.add(Karkinolution::Testing::World::make_small_world()));
	Simulation        simulation{storage};
	SimulationAdapter adapter{simulation, 17};
	SessionManager    manager{adapter};

	EXPECT_THROW(manager.receive(make_world_frame(Karkinolution::Testing::Frame::make_see_creature_frame(1, 45))),
				 std::runtime_error);
}

TEST(SessionManager, CreateWorldCallReturnsItsResultAndAcquiresTheWorldWhenRequested) {
	WorldStorage      storage;
	Simulation        simulation{storage};
	SimulationAdapter adapter{simulation, 17};
	SessionManager    manager{adapter};

	const auto payload =
		Karkinolution::Networking::Frames::System::MessageInterpreters::SystemCall::SRI::make_create_world(
			Karkinolution::Testing::World::make_small_world_preset());
	const auto extra =
		Karkinolution::Networking::Frames::System::Models::SystemCall::CreateWorldExtra{.auto_connect = true};
	manager.receive(make_system_frame(make_raw_system_call(SystemCallSubTypes::CREATE_WORLD, 3, payload, extra)));

	ASSERT_TRUE(manager.has_response());
	const auto response =
		std::get<Karkinolution::Networking::Frames::System::Models::SystemOutput::SystemOutput>(manager.pop_response());
	EXPECT_EQ(response.get_sub_type(), SystemOutputSubTypes::WorldCreated);
	EXPECT_TRUE(
		Karkinolution::Networking::Frames::System::MessageInterpreters::DSI::get_world_created(response.get_payload())
			.world_created);

	manager.receive(make_world_frame(Karkinolution::Testing::Frame::make_see_creature_frame(4, 45)));
	ASSERT_TRUE(manager.has_response());
	const auto world_response = std::get<ResponseFrame>(manager.pop_response());
	EXPECT_EQ(world_response.get_request_id(), 4);
}
