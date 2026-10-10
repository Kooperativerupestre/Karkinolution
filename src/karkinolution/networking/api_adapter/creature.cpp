
#include "karkinolution/api/creature.hpp"

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/binary/serialization/interpreters/error/alias.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"

#include <format>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/binary/serialization/interpreters/creature/alias.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>

ResponseFrame CreatureAPIBYA::see_creature(const World                  &world,
										   const std::vector<std::byte> &payload) {

	const auto id = CreatureDSI::Request::interpret_like_get_creature(payload);

	const auto request_id = Frames::Parser::Request::get_id(payload);

	auto creature = CreatureAPI::see_creature(world, id);

	if (creature == nullptr) {
		const auto error = ErrorSRI::serialize_creature_was_not_found(id);
		return Frames::Motor::make_response(BinaryTypes::Error,
											error.sub_type,
											request_id,
											error.payload);
	}
	const auto creature_payload = CreatureSRI::serialize_creature(*creature);

	return Frames::Motor::make_response(BinaryTypes::Response,
										ResponseSubTypes::CREATURE,
										request_id,
										creature_payload);
}
