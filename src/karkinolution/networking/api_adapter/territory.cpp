#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"

#include <karkinolution/api/territory.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/territory.hpp>
#include <karkinolution/networking/api_adapter/territory.hpp>

ResponseFrame Karkinolution::Networking::APIBYA::Territory::see_territory(const World &world,
																		  RequestId    request_id) {
	const auto territory = Karkinolution::API::Territory::see_territory(world);
	const auto payload   = TerritorySRI::serialize_territory(territory);
	return Karkinolution::Binary::Frames::Motor::make_response(ResponseSubTypes::TERRITORY,
															   request_id,
															   payload,
															   ResponseCardinality::ONE);
}
