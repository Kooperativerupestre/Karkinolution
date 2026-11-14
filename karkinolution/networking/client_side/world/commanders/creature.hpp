#pragma once

#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/creature.hpp"
#include "karkinolution/networking/client_side/world/aliases.hpp"
#include "karkinolution/networking/client_side/world/world.hpp"
#include "shared/math/physic/vec/model.hpp"

#include <concepts>
#include <optional>
#include <string>

namespace Karkinolution::Networking::ClientSide::World::Commanders::Creature {
	template <HasRequest T>
	bool make_creature(T                                      &client,
					   World                                  &world,
					   std::optional<Gender>                   gender,
					   std::optional<CreatureSpecies>          species,
					   std::optional<KShared::Math::Vec::Vec2> position,
					   const std::optional<std::string>       &name) {
		const auto output = Interpreters::Creature::make_creature(client, gender, species, position, name);

		if (!output) {
			return false;
		}

		return world.add(*output);
	}
} // namespace Karkinolution::Networking::ClientSide::World::Commanders::Creature
