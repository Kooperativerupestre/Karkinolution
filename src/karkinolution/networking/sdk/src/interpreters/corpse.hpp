#pragma once

#include <karkinolution/binary/interpreters/corpse.hpp>
#include <karkinolution/core/id.hpp>
#include <karkinolution/networking/sdk/src/client/client.hpp>
#include <optional>

namespace KarkinolutionSDK {
	namespace Interpreters {
		namespace Corpse {
			/*
			 * Requests a corpse from the server by id.
			 * Internally builds the request, calls the client, and parses the
			 * binary response using CorpseDSI.
			 *
			 * Returns std::nullopt when the server responds with "none" —
			 * a valid response signalling the corpse does not exist or was
			 * not found at the time of the request.
			 */
			std::optional<CorpseDSI::Models::Corpse>
			see_corpse(KarkinolutionSDK::Client::Client &client, BaseIdType id);
		} // namespace Corpse
	} // namespace Interpreters
} // namespace KarkinolutionSDK

namespace CorpseSDKInterpreter = KarkinolutionSDK::Interpreters::Corpse;
