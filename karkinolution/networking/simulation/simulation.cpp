
#include "karkinolution/world/world.hpp"
#include "shared/functional/unique_gift.hpp"

#include <karkinolution/networking/simulation/simulation.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Simulation {


			std::optional<Connection::ConnectionId>
			Simulation::get_connection_id_that_owns_world(World::WorldId id) const {
				const auto connection_id = KShared::Functional::try_at(registry, id);
				if (connection_id) {
					return *connection_id;
				}
				return std::nullopt;
			}

			bool Simulation::has_world(World::WorldId id) const {
				std::lock_guard lock{mutex};
				return registry.contains(id);
			}

			std::vector<World::WorldId> Simulation::get_all_world_ids() const {
				std::lock_guard lock{mutex};
				return worlds.get_all_world_ids();
			}

			std::optional<GiftLease<World::World>> Simulation::take_world(World::WorldId           id,
																		  Connection::ConnectionId connection_id) {
				std::lock_guard lock{mutex};
				auto            lease = worlds.take(id);
				if (lease) {
					registry[id] = connection_id;
				}
				return lease;
			}

			std::optional<WorldState> Simulation::get_world_state(World::WorldId id) const {
				std::lock_guard lock{mutex};
				const auto      connection_id = get_connection_id_that_owns_world(id);


				return WorldState{.owner = connection_id};
			}
		} // namespace Simulation
	} // namespace Networking
} // namespace Karkinolution
