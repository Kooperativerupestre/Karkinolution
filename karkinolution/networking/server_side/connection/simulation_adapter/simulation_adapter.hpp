#pragma once

#include "karkinolution/networking/config_output.hpp"
#include "karkinolution/networking/connection_id.hpp"
#include "karkinolution/networking/server_side/simulation/simulation.hpp"
#include "karkinolution/networking/world_state.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/functional/unique_gift.hpp"

#include <optional>
#include <vector>

namespace Karkinolution::Networking::ServerSide::Connection::SimulationAdapter {
	class SimulationAdapter {
		private:

			Simulation::Simulation &simulation;
			ConnectionId            connection_id;

		public:

			SimulationAdapter(Simulation::Simulation &simulation, ConnectionId connection_id)
				: simulation(simulation)
				, connection_id(connection_id) {}

			std::vector<World::WorldId> get_all_world_ids() const {
				return simulation.get_all_world_ids();
			}

			std::optional<WorldState> get_state(World::WorldId id) const {
				return simulation.get_world_state(id);
			}

			ConfigOutput config(World::WorldId new_world_id, std::optional<GiftLease<World::World>> &world_lease) {
				auto lease = simulation.take_world(new_world_id, connection_id);

				if (!lease) {
					return {.could_connect = false};
				}
				world_lease.emplace(std::move(lease.value()));
				return {.could_connect = true};
			}
	};
} // namespace Karkinolution::Networking::ServerSide::Connection::SimulationAdapter
