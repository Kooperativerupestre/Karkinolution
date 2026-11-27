#pragma once

#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/world/aliases.hpp"
#include "karkinolution/networking/client_side/world/commanders/creature.hpp"

#include <app/aliases.hpp>
#include <app/commanders/concepts.hpp>
#include <app/world_types.hpp>
#include <optional>

namespace KApp {

	namespace Commanders {
		namespace Creature {

			template <AsClient T> std::optional<WorldTypes::Creature> make_creature(T &client, World &world) {
				assert(client.is_connected());
				const auto creature =
					Karkinolution::Networking::ClientSide::World::Commanders::Creature::make_creature(client,
																									  world,
																									  std::nullopt,
																									  std::nullopt,
																									  std::nullopt,
																									  std::nullopt);
				return creature;
			}
		} // namespace Creature
	} // namespace Commanders
} // namespace KApp