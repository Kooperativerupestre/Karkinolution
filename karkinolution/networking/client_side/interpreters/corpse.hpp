#pragma once

#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/generic_id.hpp"
#include "karkinolution/networking/client_side/interpreters/internal_aliases.hpp"

#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/corpse.hpp>
#include <karkinolution/binary/interpreters/models/corpse.hpp>
#include <optional>
#include <shared/binary/deserializer.hpp>
#include <shared/core/id.hpp>

namespace Karkinolution::Networking::ClientSide::Interpreters::Corpse {
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
	see_corpse(T &client, KShared::Core::Id::BaseIdType id) {
		const auto payload = CorpseSRI::Request::make_see_corpse(id);

		const auto built =
			Karkinolution::Binary::Frames::Motor::make_request(RequestSubTypes::SEE,
															   GENERIC_INTERPRETER_ID,
															   Resource::CORPSE,
															   std::vector<std::byte>(payload.begin(), payload.end()),
															   SeeExtra{.target = SeeTarget::ONE});

		const Client::RequestData data{.sub_type = built.get_sub_type(),
									   .resource = built.get_resource(),
									   .payload  = built.get_payload(),
									   .extra    = built.get_extra()};

		const auto result = client.request(data);

		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}

		const auto &frame = require_response_frame(result.value().value());
		if (Karkinolution::Binary::Frames::Analyzer::is_none(frame)) {
			return std::nullopt;
		}

		return CorpseDSI::get_corpse(frame.get_payload(), 0);
	}

	template <HasRequest T>
	std::optional<Karkinolution::Binary::Interpreters::Models::Corpse::Corpse>
	tick_corpse(T &client, KShared::Core::Id::BaseIdType id) {
		const auto payload = CorpseSRI::Request::make_see_corpse(id);
		const auto built =
			Karkinolution::Binary::Frames::Motor::make_request(RequestSubTypes::CYCLE,
															   GENERIC_INTERPRETER_ID,
															   Resource::CORPSE,
															   std::vector<std::byte>(payload.begin(), payload.end()),
															   CycleExtra{});

		const Client::RequestData data{.sub_type = built.get_sub_type(),
									   .resource = built.get_resource(),
									   .payload  = built.get_payload(),
									   .extra    = built.get_extra()};

		const auto result = client.request(data);
		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}

		const auto &frame = require_response_frame(result.value().value());
		if (Karkinolution::Binary::Frames::Analyzer::is_none(frame)) {
			return std::nullopt;
		}

		return CorpseDSI::get_corpse(frame.get_payload(), 0);
	}

	template <HasRequest T> bool erase_corpse(T &client, KShared::Core::Id::BaseIdType id) {
		const auto payload = CorpseSRI::Request::make_see_corpse(id);
		const auto built =
			Karkinolution::Binary::Frames::Motor::make_request(RequestSubTypes::ERASE,
															   GENERIC_INTERPRETER_ID,
															   Resource::CORPSE,
															   std::vector<std::byte>(payload.begin(), payload.end()),
															   EraseExtra{});
		const Client::RequestData data{.sub_type = built.get_sub_type(),
									   .resource = built.get_resource(),
									   .payload  = built.get_payload(),
									   .extra    = built.get_extra()};
		const auto                result = client.request(data);
		if (!result || !result.value().has_value()) {
			return false;
		}
		const auto &frame = require_response_frame(result.value().value());
		if (frame.get_sub_type() != Binary::ResponseSubTypes::BOOLEAN || frame.get_payload().size() != 1) {
			throw ClientError("Unexpected response for erasing corpse");
		}
		return KShared::Binary::Deserializer::read_bool(frame.get_payload()[0]);
	}
} // namespace Karkinolution::Networking::ClientSide::Interpreters::Corpse

namespace CorpseInterpreter = Karkinolution::Networking::ClientSide::Interpreters::Corpse;
