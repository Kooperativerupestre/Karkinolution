#include "karkinolution/binary/deserialization/interpreters/creature.hpp"

#include "karkinolution/api/creature.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/binary/serialization/interpreters/creature.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"

#include <format>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>

std::vector<std::byte> CreatureAPIBYA::get_creature(const World                  &world,
													const std::vector<std::byte> &request) {

	const auto id = CreatureRequestDSI::interpret_like_get_creature(request);

	auto creature = CreatureAPI::get_creature(world, id);

	if (creature == nullptr) {
		return FrameMotor::build(
			BinarySubTypes::Error::CREATURE_WAS_NOT_FOUND,
			Serializer::convert_string(std::format("Creature with id = {}", id)));
	}
	return FrameMotor::build(BinarySubTypes::Response::CREATURE,
							 CreatureSRI::serialize_creature(*creature));
}