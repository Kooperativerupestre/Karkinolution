#pragma once

#include "karkinolution/networking/sdk/src/interpreters/concepts.hpp"
#include "karkinolution/networking/sdk/src/interpreters/generic_id.hpp"

#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/corpse.hpp>
#include <karkinolution/binary/interpreters/models/corpse.hpp>
#include <karkinolution/core/id.hpp>
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
			template <HasRequest T>
			std::optional<Karkinolution::Binary::Interpreters::Models::Corpse::Corpse>
			see_corpse(T &client, BaseIdType id) {
				const auto payload = CorpseSRI::Request::make_see_corpse(id);

				const auto built = Karkinolution::Binary::Frames::Motor::make_request(
					RequestSubTypes::SEE,
					GENERIC_INTERPRETER_ID,
					Resource::CORPSE,
					std::vector<std::byte>(payload.begin(), payload.end()));

				const KarkinolutionSDK::Client::RequestData data{.sub_type = built.sub_type,
																 .resource = built.resource,
																 .payload  = built.payload};

				const auto result = client.request(data);

				if (!result || !result.value().has_value()) {
					return std::nullopt;
				}

				const auto &frame = result.value().value();
				if (Karkinolution::Binary::Frames::Analyzer::is_none(frame)) {
					return std::nullopt;
				}

				return CorpseDSI::get_corpse(frame.payload, 0);
			}
		} // namespace Corpse
	} // namespace Interpreters
} // namespace KarkinolutionSDK

namespace CorpseSDKInterpreter = KarkinolutionSDK::Interpreters::Corpse;
