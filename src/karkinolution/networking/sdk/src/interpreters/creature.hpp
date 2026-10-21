#pragma once

#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/core/id.hpp>
#include <karkinolution/networking/sdk/src/client/client.hpp>
#include <optional>

namespace KarkinolutionSDK {
	namespace Interpreters {
		namespace Creature {
			KarkinolutionSDK::Client::ClientRequestData make_see_creature_request(BaseIdType id);

			// Returns std::nullopt when the server responds with "none" (entity not found).
			std::optional<CreatureDSI::Models::Creature>
			see_creature(KarkinolutionSDK::Client::Client &client, BaseIdType id);
		} // namespace Creature
	} // namespace Interpreters
} // namespace KarkinolutionSDK

namespace CreatureSDKInterpreter = KarkinolutionSDK::Interpreters::Creature;
