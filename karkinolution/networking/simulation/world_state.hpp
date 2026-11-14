#pragma once

#include "karkinolution/networking/connection/id.hpp"

namespace Karkinolution {
	namespace Networking {
		namespace Simulation {
			struct WorldState {
					std::optional<Connection::ConnectionId> owner;

					bool constexpr is_free() const {
						return !owner.has_value();
					}
			};
		} // namespace Simulation
	} // namespace Networking
} // namespace Karkinolution