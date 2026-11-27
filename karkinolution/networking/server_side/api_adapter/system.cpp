#include "karkinolution/networking/server_side/api_adapter/system.hpp"

#include "karkinolution/networking/frame/system/interpreter.hpp"
#include "karkinolution/networking/frame/system/message_interpreters/system.hpp"
#include "karkinolution/networking/frame/system/motor.hpp"

namespace Karkinolution::Networking::ServerSide::APIBYA::System {
	Frames::System::Models::SystemOutput::SystemOutput
	get_all_world_ids(const Frames::System::Models::SystemCall::SystemCall &call,
					  Connection::SimulationAdapter::SimulationAdapter     &simulation) {
		return Frames::System::Motor::make_system_output(
			Frames::System::SystemOutputSubTypes::AllWorldIds,
			call.get_id(),
			Frames::System::Interpreter::SRI::serialize(simulation.get_all_world_ids()));
	}

	Frames::System::Models::SystemOutput::SystemOutput
	config(const Frames::System::Models::SystemCall::SystemCall  &call,
		   Connection::SimulationAdapter::SimulationAdapter      &simulation,
		   std::optional<GiftLease<Karkinolution::World::World>> &world_lease) {
		const auto result =
			simulation.config(Frames::System::MessageInterpreters::SystemCall::DSI::get_config(call.get_payload()),
							  world_lease);
		return Frames::System::Motor::make_system_output(Frames::System::SystemOutputSubTypes::ConfigOutput,
														 call.get_id(),
														 Frames::System::Interpreter::SRI::serialize(result));
	}

	Frames::System::Models::SystemOutput::SystemOutput
	create_world(const Frames::System::Models::SystemCall::SystemCall  &call,
				 Connection::SimulationAdapter::SimulationAdapter      &simulation,
				 std::optional<GiftLease<Karkinolution::World::World>> &world_lease) {
		const auto &extra   = std::get<Frames::System::Models::SystemCall::CreateWorldExtra>(call.get_extra());
		const auto  created = simulation.create_world(
            Frames::System::MessageInterpreters::SystemCall::DSI::get_create_world(call.get_payload()),
            extra.auto_connect,
            world_lease);
		return Frames::System::Motor::make_system_output(
			Frames::System::SystemOutputSubTypes::WorldCreated,
			call.get_id(),
			Frames::System::MessageInterpreters::SystemOutput::SRI::make_world_created({.world_created = created}));
	}
} // namespace Karkinolution::Networking::ServerSide::APIBYA::System
