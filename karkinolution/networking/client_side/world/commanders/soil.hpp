#pragma once

#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/soil.hpp"
#include "karkinolution/networking/client_side/world/aliases.hpp"
#include "karkinolution/networking/client_side/world/world.hpp"

namespace Karkinolution::Networking::ClientSide::World::Commanders::Soil {
	template <HasRequest T> bool tick_soil(T &client, World &world, SoilId id) {
		const auto output = Interpreters::Soil::tick_soil(client, id);
		if (!output) {
			return false;
		}
		return world.update(*output);
	}
} // namespace Karkinolution::Networking::ClientSide::World::Commanders::Soil
