#pragma once

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/generic_id.hpp"
#include "karkinolution/networking/client_side/interpreters/internal_aliases.hpp"
#include "shared/core/id.hpp"

#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/models/soil.hpp>
#include <karkinolution/binary/interpreters/soil.hpp>
#include <optional>
#include <shared/binary/deserializer.hpp>
#include <shared/binary/types/list.hpp>
#include <vector>

namespace Karkinolution::Networking::ClientSide::Interpreters::Soil {
	template <HasRequest T>
	std::optional<Karkinolution::Binary::Interpreters::Models::Soil::Soil> see_soil(T &client,
																					KShared::Core::Id::BaseIdType id) {
		const auto payload = SoilSRI::Request::make_see_soil(id);

		const auto built =
			Karkinolution::Binary::Frames::Motor::make_request(RequestSubTypes::SEE,
															   GENERIC_INTERPRETER_ID,
															   Resource::SOIL,
															   std::vector<std::byte>(payload.begin(), payload.end()),
															   SeeExtra{.target = SeeTarget::ONE});

		const Client::RequestData    data{.sub_type = built.get_sub_type(),
										  .resource = built.get_resource(),
										  .payload  = built.get_payload(),
										  .extra    = built.get_extra()};
		const Client::ResponseResult result = client.request(data);

		if (!result || !result.value().has_value()) {
			return std::nullopt;
		}

		const auto &frame = require_response_frame(result.value().value());

		if (Karkinolution::Binary::Frames::Analyzer::is_none(frame)) {
			return std::nullopt;
		}
		return SoilDSI::get_soil(frame.get_payload(), 0);
	}

	template <HasRequest T>
	std::vector<Karkinolution::Binary::Interpreters::Models::Soil::Soil> see_all_soils(T &client) {
		const auto payload = SoilSRI::Request::make_see_all_soils();

		const auto built =
			Karkinolution::Binary::Frames::Motor::make_request(RequestSubTypes::SEE,
															   GENERIC_INTERPRETER_ID,
															   Resource::SOIL,
															   std::vector<std::byte>(payload.begin(), payload.end()),
															   SeeExtra{.target = SeeTarget::ALL});

		const Client::RequestData data{.sub_type = built.get_sub_type(),
									   .resource = built.get_resource(),
									   .payload  = built.get_payload(),
									   .extra    = built.get_extra()};
		const auto                result = client.request(data);

		if (!result || !result.value().has_value()) {
			return {};
		}

		const auto &frame = require_response_frame(result.value().value());
		if (Karkinolution::Binary::Frames::Analyzer::is_none(frame)) {
			return {};
		}

		return ListTDSI::deserialize<SoilSRI::Layout::TOTAL_SIZE>(frame.get_payload(), SoilDSI::get_soil, 0);
	}

	template <HasRequest T>
	std::optional<Karkinolution::Binary::Interpreters::Models::Soil::Soil> tick_soil(T &client,
																					 KShared::Core::Id::BaseIdType id) {
		const auto payload = SoilSRI::Request::make_see_soil(id);
		const auto built =
			Karkinolution::Binary::Frames::Motor::make_request(RequestSubTypes::CYCLE,
															   GENERIC_INTERPRETER_ID,
															   Resource::SOIL,
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

		return SoilDSI::get_soil(frame.get_payload(), 0);
	}

	template <HasRequest T> bool erase_soil(T &client, KShared::Core::Id::BaseIdType id) {
		const auto payload = SoilSRI::Request::make_see_soil(id);
		const auto built =
			Karkinolution::Binary::Frames::Motor::make_request(RequestSubTypes::ERASE,
															   GENERIC_INTERPRETER_ID,
															   Resource::SOIL,
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
			throw ClientError("Unexpected response for erasing soil");
		}
		return KShared::Binary::Deserializer::read_bool(frame.get_payload()[0]);
	}
} // namespace Karkinolution::Networking::ClientSide::Interpreters::Soil

namespace SoilInterpreter = Karkinolution::Networking::ClientSide::Interpreters::Soil;
