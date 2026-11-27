#include "karkinolution/networking/frame/system/core_aliases.hpp"
#include "karkinolution/networking/frame/system/models.hpp"
#include "karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp"

#include <karkinolution/networking/frame/system/processor.hpp>
#include <karkinolution/networking/server_side/api_adapter/system.hpp>
#include <shared/functional/unreachable_or_throw.hpp>

namespace Karkinolution::Networking::Frames::System {
	Frames::System::Models::SystemOutput::SystemOutput
	Processor::process(const Frames::System::Models::SystemCall::SystemCall         &call,
					   ServerSide::Connection::SimulationAdapter::SimulationAdapter &simulation,
					   std::optional<GiftLease<Karkinolution::World::World>>        &world_lease) {
		const auto sub_type = call.get_sub_type();

		if (sub_type == Frames::System::SystemCallSubTypes::GET_ALL_WORLD_IDS) {
			return ServerSide::APIBYA::System::get_all_world_ids(call, simulation);
		} else if (sub_type == Frames::System::SystemCallSubTypes::CONFIG) {
			return ServerSide::APIBYA::System::config(call, simulation, world_lease);
		} else if (sub_type == Frames::System::SystemCallSubTypes::CREATE_WORLD) {
			return ServerSide::APIBYA::System::create_world(call, simulation, world_lease);
		}
		KShared::Functional::unreachable_or_throw("Invalid sub type of SystemCall");
	}
} // namespace Karkinolution::Networking::Frames::System
