#include "karkinolution/binary/deserialization/interpreters/models.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature.hpp>
#include <karkinolution/binary/message_type_size.hpp>

Gender CreatureResponseDSI::get_gender(const std::vector<std::byte> &bytes) {
	if (bytes[0] == std::byte(0x01)) {
		return Gender::FEMALE;
	} else {
		return Gender::MALE;
	}
}

CreatureSpecies CreatureResponseDSI::get_specie(const std::vector<std::byte> &payload) {
	if (payload[1] == std::byte(0x01)) {
		return CreatureSpecies::CRAB;
	} else if (payload[1] == std::byte(0x02)) {
		return CreatureSpecies::HIPPOPOTAMUS;
	} else if (payload[1] == std::byte(0x03)) {
		return CreatureSpecies::CROCODILE;
	} else {
		return CreatureSpecies::FISH;
	}
}

DesserializedCreature CreatureResponseDSI::get_creature(const std::vector<std::byte> &payload) {
	return DesserializedCreature{.gender = get_gender(payload), .specie = get_specie(payload)};
}

BaseIdType CreatureRequestDSI::interpret_like_get_creature(const std::vector<std::byte> &payload) {
	return BaseIdType{Deserializer::read_uint64_t(payload, 0)};
}
