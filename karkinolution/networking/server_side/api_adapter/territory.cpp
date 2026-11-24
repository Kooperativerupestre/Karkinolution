#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"

#include <karkinolution/api/territory.hpp>
#include <karkinolution/networking/frame/world/interpreters/territory.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <karkinolution/networking/server_side/api_adapter/territory.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Territory {
	ResponseFrame see_territory(const Karkinolution::World::World &world, Frames::World::RequestId request_id) {
		const auto territory = Karkinolution::API::Territory::see_territory(world);
		const auto payload   = TerritorySRI::serialize_territory(territory);
		return Karkinolution::Networking::Frames::World::Motor::make_response(
			Frames::World::ResponseSubTypes::TERRITORY,
			request_id,
			payload,
			Frames::World::ResponseCardinality::ONE);
	}
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Territory
