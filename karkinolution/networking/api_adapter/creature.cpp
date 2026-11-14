
#include "karkinolution/api/creature.hpp"

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/interpreters/error.hpp"
#include "shared/binary/serializer.hpp"

#include <format>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>

ResponseFrame Karkinolution::Networking::APIBYA::Creature::see_creature(const Karkinolution::World::World &world,
																		Binary::RequestId                  request_id,
																		const std::vector<std::byte>      &payload) {

	const auto id = CreatureDSI::Request::interpret_like_get_creature(payload);


	auto creature = Karkinolution::API::Creature::see_creature(world, id);

	if (creature == nullptr) {

		return Karkinolution::Binary::Frames::Motor::make_none_response(request_id);
	}
	const auto creature_payload = CreatureSRI::serialize_creature(*creature);

	return Karkinolution::Binary::Frames::Motor::make_response(Binary::ResponseSubTypes::CREATURE,
															   request_id,
															   creature_payload,
															   Binary::ResponseCardinality::ONE);
}

ResponseFrame Karkinolution::Networking::APIBYA::Creature::create_creature(Karkinolution::World::World  &world,
																		   Binary::RequestId             request_id,
																		   const std::vector<std::byte> &payload) {
	const auto blueprint = CreatureDSI::Request::interpret_like_create_creature(payload);

	auto creature = Karkinolution::API::Creature::make_creature(world,
																blueprint.gender,
																blueprint.specie,
																blueprint.position,
																blueprint.name);
	if (creature == nullptr) {
		return Karkinolution::Binary::Frames::Motor::make_none_response(request_id);
	}

	const auto creature_payload = CreatureSRI::serialize_creature(*creature);

	return Karkinolution::Binary::Frames::Motor::make_response(Binary::ResponseSubTypes::CREATURE,
															   request_id,
															   creature_payload,
															   Binary::ResponseCardinality::ONE);
}
