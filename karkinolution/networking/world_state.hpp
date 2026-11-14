#pragma once

#include "karkinolution/networking/connection_id.hpp"

#include <optional>

namespace Karkinolution::Networking {
	struct WorldState {
			std::optional<ConnectionId> owner;

			bool constexpr is_free() const {
				return !owner.has_value();
			}
	};
} // namespace Karkinolution::Networking
