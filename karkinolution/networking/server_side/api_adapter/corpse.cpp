#include "karkinolution/networking/frame/world/core_aliases.hpp"

#include <karkinolution/api/corpse.hpp>
#include <karkinolution/networking/frame/world/interpreters/corpse.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <karkinolution/networking/server_side/api_adapter/corpse.hpp>
#include <shared/binary/serializer.hpp>
#include <shared/binary/types/list.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Corpse {
	ResponseFrame see_corpse(const Karkinolution::World::World &world,
							 Frames::World::RequestId           request_id,
							 const std::vector<std::byte>      &payload) {
		const auto id = CorpseDSI::Request::interpret_like_see_corpse(payload);

		auto corpse = Karkinolution::API::Corpse::see_corpse(world, id);

		if (!corpse) {
			return Karkinolution::Networking::Frames::World::Motor::make_none_response(request_id);
		}
		const auto corpse_payload = CorpseSRI::serialize_corpse(*corpse);

		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::CORPSE,
																			  request_id,
																			  corpse_payload,
																			  Frames::World::ResponseCardinality::ONE);
	}

	ResponseFrame see_all_corpses(const Karkinolution::World::World &world, Frames::World::RequestId request_id) {
		const auto ids = Karkinolution::API::Corpse::see_all_corpse_ids(world);
		const auto payload =
			ListTSRI::serialize<sizeof(KShared::Core::Id::BaseIdType)>(ids, [](KShared::Core::Id::BaseIdType id) {
				return KShared::Binary::Serializer::serialize(id);
			});
		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::CORPSE,
																			  request_id,
																			  payload,
																			  Frames::World::ResponseCardinality::ALL);
	}

	ResponseFrame tick_corpse(Karkinolution::World::World  &world,
							  Frames::World::RequestId      request_id,
							  const std::vector<std::byte> &payload) {
		const auto id     = CorpseDSI::Request::interpret_like_see_corpse(payload);
		const auto corpse = Karkinolution::API::Corpse::tick(world, id);

		if (!corpse) {
			return Karkinolution::Networking::Frames::World::Motor::make_none_response(request_id);
		}

		const auto corpse_payload = CorpseSRI::serialize_corpse(*corpse);
		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::CORPSE,
																			  request_id,
																			  corpse_payload,
																			  Frames::World::ResponseCardinality::ONE);
	}

	ResponseFrame erase_corpse(Karkinolution::World::World  &world,
							   Frames::World::RequestId      request_id,
							   const std::vector<std::byte> &payload) {
		const auto id = CorpseDSI::Request::interpret_like_see_corpse(payload);
		return Karkinolution::Networking::Frames::World::Motor::make_boolean_response(
			request_id,
			Karkinolution::API::Corpse::erase(world, id));
	}
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Corpse
