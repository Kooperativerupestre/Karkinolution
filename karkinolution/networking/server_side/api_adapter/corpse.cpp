#include "karkinolution/binary/core_aliases.hpp"

#include <karkinolution/api/corpse.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/corpse.hpp>
#include <karkinolution/networking/server_side/api_adapter/corpse.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Corpse {
	ResponseFrame see_corpse(const Karkinolution::World::World &world,
							 Binary::RequestId                  request_id,
							 const std::vector<std::byte>      &payload) {
		const auto id = CorpseDSI::Request::interpret_like_see_corpse(payload);

		auto corpse = Karkinolution::API::Corpse::see_corpse(world, id);

		if (corpse == nullptr) {
			return Karkinolution::Binary::Frames::Motor::make_none_response(request_id);
		}
		const auto corpse_payload = CorpseSRI::serialize_corpse(*corpse);

		return Karkinolution::Binary::Frames::Motor::make_response(Binary::ResponseSubTypes::CORPSE,
																   request_id,
																   corpse_payload,
																   Binary::ResponseCardinality::ONE);
	}
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Corpse
