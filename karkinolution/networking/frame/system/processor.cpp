#include "karkinolution/networking/frame/system/core_aliases.hpp"
#include "karkinolution/networking/frame/system/models.hpp"
#include "karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/networking/server_side/simulation/simulation.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/functional/unique_gift.hpp"

#include <karkinolution/networking/frame/system/interpreter.hpp>
#include <karkinolution/networking/frame/system/motor.hpp>
#include <karkinolution/networking/frame/system/processor.hpp>
#include <shared/functional/unreachable_or_throw.hpp>

namespace Karkinolution::Networking::Frames::System {
	Frames::System::Models::SystemOutput::SystemOutput
	Processor::process(const Frames::System::Models::SystemCall::SystemCall         &call,
					   ServerSide::Connection::SimulationAdapter::SimulationAdapter &simulation,
					   std::optional<GiftLease<World::World>>                       &world_lease) {
		const auto sub_type = call.get_sub_type();

		if (sub_type == Frames::System::SystemCallSubTypes::GET_ALL_WORLD_IDS) {
			return Motor::make_system_output(Frames::System::SystemOutputSubTypes::AllWorldIds,
											 call.get_id(),
											 Interpreter::SRI::serialize(simulation.get_all_world_ids()));
		} else if (sub_type == Frames::System::SystemCallSubTypes::CONFIG) {
			return Motor::make_system_output(
				Frames::System::SystemOutputSubTypes::ConfigOutput,
				call.get_id(),
				Interpreter::SRI::serialize(
					simulation.config(Interpreter::SystemCall::DSI::get_config(call.get_payload()), world_lease)));
		} else if (sub_type == Frames::System::SystemCallSubTypes::CREATE_WORLD) {
			const auto &extra = std::get<Frames::System::Models::SystemCall::CreateWorldExtra>(call.get_extra());
			const auto  created =
				simulation.create_world(Interpreter::SystemCall::DSI::get_create_world(call.get_payload()),
										extra.auto_connect,
										world_lease);
			return Motor::make_system_output(
				Frames::System::SystemOutputSubTypes::WorldCreated,
				call.get_id(),
				Interpreter::SystemOutput::SRI::make_world_created({.world_created = created}));
		}
		KShared::Functional::unreachable_or_throw("Invalid sub type of SystemCall");
	}
} // namespace Karkinolution::Networking::Frames::System
