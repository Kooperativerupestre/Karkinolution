#pragma once

#include "karkinolution/networking/frame/system/models.hpp"
#include "karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/functional/unique_gift.hpp"

#include <optional>

namespace Karkinolution::Networking::ServerSide::APIBYA::System {
	Frames::System::Models::SystemOutput::SystemOutput
	get_all_world_ids(const Frames::System::Models::SystemCall::SystemCall &call,
					  Connection::SimulationAdapter::SimulationAdapter     &simulation);

	Frames::System::Models::SystemOutput::SystemOutput
	config(const Frames::System::Models::SystemCall::SystemCall  &call,
		   Connection::SimulationAdapter::SimulationAdapter      &simulation,
		   std::optional<GiftLease<Karkinolution::World::World>> &world_lease);

	Frames::System::Models::SystemOutput::SystemOutput
	create_world(const Frames::System::Models::SystemCall::SystemCall  &call,
				 Connection::SimulationAdapter::SimulationAdapter      &simulation,
				 std::optional<GiftLease<Karkinolution::World::World>> &world_lease);
} // namespace Karkinolution::Networking::ServerSide::APIBYA::System
