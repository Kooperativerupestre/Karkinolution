#pragma once

#include "karkinolution/networking/connection_id.hpp"
#include "karkinolution/networking/world_state.hpp"
#include "karkinolution/world/world.hpp"
#include "karkinolution/world/world_storage.hpp"
#include "shared/functional/data_structures.hpp"
#include "shared/functional/optional_reference.hpp"
#include "shared/functional/unique_gift.hpp"
#include "shared/math/stats/compile_values.hpp"

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace Karkinolution::Networking::ServerSide::Simulation {
	class Simulation {
		private:

			mutable std::mutex mutex;

			World::WorldStorage &worlds;

			std::unordered_map<World::WorldId, ConnectionId> registry;

			std::optional<ConnectionId> get_connection_id_that_owns_world(World::WorldId id) const;

		public:

			explicit Simulation(World::WorldStorage &worlds)
				: worlds(worlds) {}

			bool has_world(World::WorldId id) const;

			std::vector<World::WorldId>            get_all_world_ids() const;
			std::optional<GiftLease<World::World>> take_world(World::WorldId id, ConnectionId connection_id);
			std::optional<WorldState>              get_world_state(World::WorldId id) const;
	};
} // namespace Karkinolution::Networking::ServerSide::Simulation
