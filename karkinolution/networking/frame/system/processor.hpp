#pragma once

#include "karkinolution/networking/frame/system/models.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/functional/unique_gift.hpp"

#include <cstddef>
#include <karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::System {
			namespace Processor {
				Frames::System::Models::SystemOutput::SystemOutput
				process(const Frames::System::Models::SystemCall::SystemCall         &call,
						ServerSide::Connection::SimulationAdapter::SimulationAdapter &simulation,
						std::optional<GiftLease<Karkinolution::World::World>>        &world_lease);
			} // namespace Processor
		} // namespace Frames::System
	} // namespace Networking
} // namespace Karkinolution
