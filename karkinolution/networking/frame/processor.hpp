#pragma once

#include "karkinolution/networking/frame/models.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/functional/unique_gift.hpp"

#include <cstddef>
#include <karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Processor {
				Frames::Models::SystemOutput::SystemOutput
				process(const Frames::Models::SystemCall::SystemCall                 &call,
						ServerSide::Connection::SimulationAdapter::SimulationAdapter &simulation,
						std::optional<GiftLease<World::World>>                       &world_lease);
			} // namespace Processor
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution
