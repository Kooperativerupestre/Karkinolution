#include "karkinolution/networking/connection/protocol_sessions/session_manager.hpp"

#include "karkinolution/networking/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/networking/frame/interpreter.hpp"
#include "karkinolution/networking/frame/motor.hpp"
#include "karkinolution/networking/scope/motor.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/networking/simulation/simulation.hpp"
#include "karkinolution/test/utils.hpp"
#include "karkinolution/world/world_storage.hpp"

#include <gtest/gtest.h>
#include <variant>

namespace {
	using ::ResponseFrame;
	using Karkinolution::Networking::Connection::ProtocolSessions::SessionManager;
	using Karkinolution::Networking::Connection::SimulationAdapter::SimulationAdapter;
	using Karkinolution::Networking::Frames::SystemCallSubTypes;
	using Karkinolution::Networking::Frames::SystemOutputSubTypes;
	using Karkinolution::Networking::Frames::Motor::make_raw_system_call;
	using Karkinolution::Networking::Scope::Motor::make_system_frame;
	using Karkinolution::Networking::Scope::Motor::make_world_frame;
	using Karkinolution::Networking::Simulation::Simulation;
	using Karkinolution::World::WorldStorage;

	Karkinolution::World::World make_world() {
		return TestUtils::make_small_world();
	}
} // namespace

TEST(SessionManager, RoutesSystemFramesAndWorldFramesWithoutForwardingScope) {
	WorldStorage storage;
	ASSERT_TRUE(storage.add(make_world()));
	const auto        world_id = storage.get_all_world_ids().front();
	Simulation        simulation{storage};
	SimulationAdapter adapter{simulation, 17};
	SessionManager    manager{adapter};

	const auto world_list_call =
		make_system_frame(make_raw_system_call(SystemCallSubTypes::GET_ALL_WORLD_IDS, 1, std::vector<std::byte>{}));
	manager.receive(world_list_call);
	ASSERT_TRUE(manager.has_response());
	const auto world_list_response =
		std::get<Karkinolution::Networking::Frames::Models::SystemOutput::SystemOutput>(manager.pop_response());
	EXPECT_EQ(world_list_response.get_call_id(), 1);
	EXPECT_EQ(world_list_response.get_sub_type(), SystemOutputSubTypes::AllWorldIds);

	const auto config_payload = Karkinolution::Networking::Frames::Interpreter::SystemCall::SRI::make_config(world_id);
	manager.receive(make_system_frame(make_raw_system_call(SystemCallSubTypes::CONFIG, 2, config_payload)));
	ASSERT_TRUE(manager.has_response());
	const auto config_response =
		std::get<Karkinolution::Networking::Frames::Models::SystemOutput::SystemOutput>(manager.pop_response());
	EXPECT_EQ(config_response.get_sub_type(), SystemOutputSubTypes::ConfigOutput);
	EXPECT_TRUE(Karkinolution::Networking::Frames::Interpreter::DSI::get_config_output(config_response.get_payload())
					.could_connect);

	const auto world_request = make_world_frame(TestUtils::make_see_creature_frame(3, 45));
	manager.receive(world_request);
	ASSERT_TRUE(manager.has_response());
	const auto world_response = std::get<ResponseFrame>(manager.pop_response());
	EXPECT_EQ(world_response.get_request_id(), 3);
}

TEST(SessionManager, RejectsWorldFramesBeforeWorldAcquisition) {
	WorldStorage storage;
	ASSERT_TRUE(storage.add(make_world()));
	Simulation        simulation{storage};
	SimulationAdapter adapter{simulation, 17};
	SessionManager    manager{adapter};

	EXPECT_THROW(manager.receive(make_world_frame(TestUtils::make_see_creature_frame(1, 45))), std::runtime_error);
}
