#pragma once
#include "karkinolution/networking/sdk/src/interpreters/creature.hpp"
#include "shared/math/physic/vec/model.hpp"

#include <concepts>
#include <karkinolution/networking/sdk/src/interpreters/concepts.hpp>
#include <karkinolution/networking/sdk/src/world/aliases.hpp>
#include <karkinolution/networking/sdk/src/world/world.hpp>
#include <optional>

namespace KarkinolutionSDK {
	namespace World {
		namespace Commanders {
			namespace Creature {

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
			} // namespace Creature
		} // namespace Commanders
	} // namespace World
} // namespace KarkinolutionSDK