#pragma once

#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/core/id.hpp>
#include <karkinolution/networking/sdk/src/client/client.hpp>
#include <optional>

namespace KarkinolutionSDK {
	namespace Interpreters {
		namespace Creature {
			// Returns std::nullopt when the server responds with "none" (entity not found).
			std::optional<CreatureDSI::Models::Creature>
			see_creature(KarkinolutionSDK::Client::Client &client, BaseIdType id);
			std::optional<CreatureDSI::Models::Creature>
			make_creature(KarkinolutionSDK::Client::Client &client,
						  std::optional<Gender>             gender,
						  std::optional<CreatureSpecies>    specie,
						  const std::optional<Vec3>        &position,
						  const std::optional<std::string> &name);
		} // namespace Creature
	} // namespace Interpreters
} // namespace KarkinolutionSDK

namespace CreatureSDKInterpreter = KarkinolutionSDK::Interpreters::Creature;
