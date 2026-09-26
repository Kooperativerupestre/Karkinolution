#include "karkinolution/binary/serialization/interpreters/creature.hpp"

#include "karkinolution/binary/deserialization/interpreters/models.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature.hpp>
#include <karkinolution/binary/message_type_size.hpp>

Gender CreatureResponseDSI::get_gender(const std::vector<std::byte> &bytes) {
	if (bytes[CreatureSRI::TO_GET_GENDER_OFFSET] == std::byte(0x01)) {
		return Gender::FEMALE;
	} else {
		return Gender::MALE;
	}
}

CreatureSpecies CreatureResponseDSI::get_specie(const std::vector<std::byte> &payload) {
	if (payload[CreatureSRI::TO_GET_SPECIE_OFFSET] == std::byte(0x01)) {
		return CreatureSpecies::CRAB;
	} else if (payload[CreatureSRI::TO_GET_SPECIE_OFFSET] == std::byte(0x02)) {
		return CreatureSpecies::HIPPOPOTAMUS;
	} else if (payload[CreatureSRI::TO_GET_SPECIE_OFFSET] == std::byte(0x03)) {
		return CreatureSpecies::CROCODILE;
	} else {
		return CreatureSpecies::FISH;
	}
}

double CreatureResponseDSI::get_position_x(const std::vector<std::byte> &payload) {
	return Deserializer::read_double(payload, CreatureSRI::TO_GET_POSITION_X_OFFSET);
}

double CreatureResponseDSI::get_position_y(const std::vector<std::byte> &payload) {
	return Deserializer::read_double(payload, CreatureSRI::TO_GET_POSITION_Y_OFFSET);
}

double CreatureResponseDSI::get_position_z(const std::vector<std::byte> &payload) {
	return Deserializer::read_double(payload, CreatureSRI::TO_GET_POSITION_Z_OFFSET);
}

Vec3 CreatureResponseDSI::get_position(const std::vector<std::byte> &payload) {
	return Vec3{get_position_x(payload), get_position_y(payload), get_position_z(payload)};
}

DesserializedCreature CreatureResponseDSI::get_creature(const std::vector<std::byte> &payload) {
	return DesserializedCreature{.gender   = get_gender(payload),
								 .specie   = get_specie(payload),
								 .position = get_position(payload),
								 .name     = get_name(payload)};
}

std::string CreatureResponseDSI::get_name(const std::vector<std::byte> &payload) {
	return Deserializer::read_string(payload,
									 CreatureSRI::TO_GET_NAME_OFFSET,
									 CreatureSRI::NAME_BYTES);
}

BaseIdType CreatureRequestDSI::interpret_like_get_creature(const std::vector<std::byte> &payload) {
	return BaseIdType{Deserializer::read_uint64_t(payload, 0)};
}
