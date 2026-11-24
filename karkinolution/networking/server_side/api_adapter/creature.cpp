#include "karkinolution/api/creature.hpp"

#include "karkinolution/networking/frame/world/aliases.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "karkinolution/networking/frame/world/interpreters/error.hpp"
#include "shared/binary/serializer.hpp"

#include <format>
#include <karkinolution/networking/frame/world/interpreters/creature.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>
#include <karkinolution/networking/frame/world/parser/parser.hpp>
#include <karkinolution/networking/server_side/api_adapter/creature.hpp>

namespace Karkinolution::Networking::ServerSide::APIBYA::Creature {
	ResponseFrame see_creature(const Karkinolution::World::World &world,
							   Frames::World::RequestId           request_id,
							   const std::vector<std::byte>      &payload) {
		const auto id = CreatureDSI::Request::interpret_like_get_creature(payload);

		auto creature = Karkinolution::API::Creature::see_creature(world, id);

		if (!creature) {
			return Karkinolution::Networking::Frames::World::Motor::make_none_response(request_id);
		}
		const auto creature_payload = CreatureSRI::serialize_creature(*creature);

		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::CREATURE,
																			  request_id,
																			  creature_payload,
																			  Frames::World::ResponseCardinality::ONE);
	}

	ResponseFrame create_creature(Karkinolution::World::World  &world,
								  Frames::World::RequestId      request_id,
								  const std::vector<std::byte> &payload) {
		const auto blueprint = CreatureDSI::Request::interpret_like_create_creature(payload);

		auto creature = Karkinolution::API::Creature::make_creature(world,
																	blueprint.gender,
																	blueprint.specie,
																	blueprint.position,
																	blueprint.name);
		if (!creature) {
			return Karkinolution::Networking::Frames::World::Motor::make_none_response(request_id);
		}

		const auto creature_payload = CreatureSRI::serialize_creature(*creature);

		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::CREATURE,
																			  request_id,
																			  creature_payload,
																			  Frames::World::ResponseCardinality::ONE);
	}

	ResponseFrame tick_creature(Karkinolution::World::World  &world,
								Frames::World::RequestId      request_id,
								const std::vector<std::byte> &payload) {
		const auto id       = CreatureDSI::Request::interpret_like_get_creature(payload);
		const auto creature = Karkinolution::API::Creature::tick(world, id);

		if (!creature) {
			return Karkinolution::Networking::Frames::World::Motor::make_none_response(request_id);
		}

		const auto creature_payload = CreatureSRI::serialize_creature(*creature);
		return Karkinolution::Networking::Frames::World::Motor::make_response(Frames::World::ResponseSubTypes::CREATURE,
																			  request_id,
																			  creature_payload,
																			  Frames::World::ResponseCardinality::ONE);
	}

	ResponseFrame erase_creature(Karkinolution::World::World  &world,
								 Frames::World::RequestId      request_id,
								 const std::vector<std::byte> &payload) {
		const auto id = CreatureDSI::Request::interpret_like_get_creature(payload);
		if (!Karkinolution::API::Creature::erase(world, id)) {
			return Karkinolution::Networking::Frames::World::Motor::make_boolean_response(
				request_id,
				Karkinolution::API::Creature::erase(world, id));
		}
		return Karkinolution::Networking::Frames::World::Motor::make_none_response(request_id);
	}
} // namespace Karkinolution::Networking::ServerSide::APIBYA::Creature
