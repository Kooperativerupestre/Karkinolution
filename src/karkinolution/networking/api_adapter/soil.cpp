#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"

#include <karkinolution/api/soil.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/soil.hpp>
#include <karkinolution/networking/api_adapter/soil.hpp>

ResponseFrame
Karkinolution::Networking::APIBYA::Soil::see_soil(const World                  &world,
												  RequestId                     request_id,
												  const std::vector<std::byte> &payload) {
	const auto id = SoilDSI::Request::interpret_like_get_soil(payload);

	auto soil = Karkinolution::API::Soil::see_soil(world, id);

	if (soil == nullptr) {
		return Frames::Motor::make_none_response(request_id);
	}
	const auto soil_payload = SoilSRI::serialize_soil(*soil);
	return Frames::Motor::make_response(BinaryTypes::Response,
										ResponseSubTypes::SOIL,
										request_id,
										soil_payload);
}