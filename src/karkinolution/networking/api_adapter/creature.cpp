
#include "karkinolution/api/creature.hpp"

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/interpreters/error.hpp"
#include "karkinolution/binary/serializer.hpp"

#include <format>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>

// TODO: add unit test to verify if a creature that exists and a creature that doesn't exist, don't
// send error

ResponseFrame CreatureAPIBYA::see_creature(const World                  &world,
										   RequestId                     request_id,
										   const std::vector<std::byte> &payload) {

	const auto id = CreatureDSI::Request::interpret_like_get_creature(payload);


	auto creature = CreatureAPI::see_creature(world, id);

	if (creature == nullptr) {

		return Frames::Motor::make_response(BinaryTypes::Response,
											ResponseSubTypes::NONE,
											request_id,
											std::vector<std::byte>{});
	}
	const auto creature_payload = CreatureSRI::serialize_creature(*creature);

	return Frames::Motor::make_response(BinaryTypes::Response,
										ResponseSubTypes::CREATURE,
										request_id,
										creature_payload);
}
