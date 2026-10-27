#include "karkinolution/binary/interpreters/models/soil.hpp"

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/terrain/soil.hpp"

#include <karkinolution/api/soil.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/soil.hpp>
#include <karkinolution/binary/types/list.hpp>
#include <karkinolution/networking/api_adapter/soil.hpp>

ResponseFrame
Karkinolution::Networking::APIBYA::Soil::see_soil(const World                  &world,
												  RequestId                     request_id,
												  const std::vector<std::byte> &payload) {
	const auto id = SoilDSI::Request::interpret_like_get_soil(payload);

	auto soil = Karkinolution::API::Soil::see_soil(world, id);

	if (soil == nullptr) {
		return Karkinolution::Binary::Frames::Motor::make_none_response(request_id);
	}
	const auto soil_payload = SoilSRI::serialize_soil(*soil);
	return Karkinolution::Binary::Frames::Motor::make_response(ResponseSubTypes::SOIL,
															   request_id,
															   soil_payload,
															   ResponseCardinality::ONE);
}

ResponseFrame
Karkinolution::Networking::APIBYA::Soil::see_all_soils(const World                  &world,
													   RequestId                     request_id,
													   const std::vector<std::byte> &payload) {
	auto all_soils = Karkinolution::API::Soil::see_all_soils(world);

	std::vector<std::byte> r_payload =
		Binary::Types::List::SRI::serialize<SoilSRI::Layout::TOTAL_SIZE>(
			all_soils,
			[](const SoilPiece &soil) {
				return SoilSRI::serialize_soil(soil);
			});

	return Karkinolution::Binary::Frames::Motor::make_response(ResponseSubTypes::SOIL,
															   request_id,
															   r_payload,
															   ResponseCardinality::ALL);
}