#pragma once

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/networking/sdk/src/interpreters/concepts.hpp"
#include "karkinolution/networking/sdk/src/interpreters/generic_id.hpp"

#include <karkinolution/binary/frames/analyzer.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/models/soil.hpp>
#include <karkinolution/binary/interpreters/soil.hpp>

namespace KarkinolutionSDK {
	namespace Interpreters {
		namespace Soil {

			template <HasRequest T>
			std::optional<Karkinolution::Binary::Interpreters::Models::Soil::Soil>
			see_soil(T &client, BaseIdType id) {
				const auto payload = SoilSRI::Request::make_see_soil(id);

				const auto built = Karkinolution::Binary::Frames::Motor::make_request(
					RequestSubTypes::SEE,
					GENERIC_INTERPRETER_ID,
					Resource::SOIL,
					std::vector<std::byte>(payload.begin(), payload.end()));

				const KarkinolutionSDK::Client::RequestData    data{.sub_type = built.sub_type,
																	.resource = built.resource,
																	.payload  = built.payload};
				const KarkinolutionSDK::Client::ResponseResult result = client.request(data);

				if (!result || !result.value().has_value()) {
					return std::nullopt;
				}

				const auto &frame = result.value().value();

				if (Karkinolution::Binary::Frames::Analyzer::is_none(frame)) {
					return std::nullopt;
				}
				return SoilDSI::get_soil(frame.payload, 0);
			}
		} // namespace Soil
	} // namespace Interpreters
} // namespace KarkinolutionSDK