#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/frame/models.hpp"
#include "karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/networking/server_side/simulation/simulation.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/functional/unique_gift.hpp"

#include <karkinolution/networking/frame/interpreter.hpp>
#include <karkinolution/networking/frame/motor.hpp>
#include <karkinolution/networking/frame/processor.hpp>
#include <shared/functional/unreachable_or_throw.hpp>

namespace Karkinolution::Networking::Frames {
	Frames::Models::SystemOutput::SystemOutput
	Processor::process(const Frames::Models::SystemCall::SystemCall                 &call,
					   ServerSide::Connection::SimulationAdapter::SimulationAdapter &simulation,
					   std::optional<GiftLease<World::World>>                       &world_lease) {
		const auto sub_type = call.get_sub_type();

		if (sub_type == Frames::SystemCallSubTypes::GET_ALL_WORLD_IDS) {
			return Motor::make_system_output(Frames::SystemOutputSubTypes::AllWorldIds,
											 call.get_id(),
											 Interpreter::SRI::serialize(simulation.get_all_world_ids()));
		} else if (sub_type == Frames::SystemCallSubTypes::CONFIG) {
			return Motor::make_system_output(
				Frames::SystemOutputSubTypes::ConfigOutput,
				call.get_id(),
				Interpreter::SRI::serialize(
					simulation.config(Interpreter::SystemCall::DSI::get_config(call.get_payload()), world_lease)));
		}
		KShared::Functional::unreachable_or_throw("Invalid sub type of SystemCall");
	}
} // namespace Karkinolution::Networking::Frames
