#pragma once

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/analyzer.hpp"
#include "karkinolution/binary/frames/motor.hpp"
#include "karkinolution/binary/interpreters/models/territory.hpp"
#include "karkinolution/binary/interpreters/territory.hpp"
#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/generic_id.hpp"
#include "karkinolution/networking/client_side/interpreters/internal_aliases.hpp"

#include <optional>
#include <vector>

namespace Karkinolution::Networking::ClientSide::Interpreters::Territory {
	template <HasRequest T>
	std::optional<Karkinolution::Binary::Interpreters::Models::Territory::Territory> see_territory(T &client) {
		const auto payload = TerritorySRI::Request::make_see_territory();
		const auto built =
			Karkinolution::Binary::Frames::Motor::make_request(RequestSubTypes::SEE,
															   GENERIC_INTERPRETER_ID,
															   Resource::TERRITORY,
															   std::vector<std::byte>(payload.begin(), payload.end()),
															   SeeExtra{.target = SeeTarget::ONE});

		const Client::RequestData data{.sub_type = built.get_sub_type(),
									   .resource = built.get_resource(),
									   .payload  = built.get_payload(),
									   .extra    = built.get_extra()};
		const auto                result = client.request(data);
		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}

		const auto &frame = require_response_frame(result.value().value());
		if (Karkinolution::Binary::Frames::Analyzer::is_none(frame)) {
			return std::nullopt;
		}
		return TerritoryDSI::deserialize_territory(frame.get_payload(), 0);
	}
} // namespace Karkinolution::Networking::ClientSide::Interpreters::Territory

namespace TerritoryInterpreter = Karkinolution::Networking::ClientSide::Interpreters::Territory;
