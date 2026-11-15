#pragma once

#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/corpse.hpp"
#include "karkinolution/networking/client_side/world/aliases.hpp"
#include "karkinolution/networking/client_side/world/world.hpp"

namespace Karkinolution::Networking::ClientSide::World::Commanders::Corpse {
	template <HasRequest T> bool tick_corpse(T &client, World &world, OrganismId id) {
		const auto output = Interpreters::Corpse::tick_corpse(client, id);
		if (!output) {
			return false;
		}
		return world.update(*output);
	}
} // namespace Karkinolution::Networking::ClientSide::World::Commanders::Corpse
