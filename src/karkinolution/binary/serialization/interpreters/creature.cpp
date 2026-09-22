#include "karkinolution/binary/serialization/interpreters/creature.hpp"

#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <format>

using CreatureSRI::CreatureBytes;

using CreatureSRI::CreatureBytes;

std::byte CreatureSRI::serialize_gender(const Gender &gender) {
	if (gender == Gender::FEMALE) {
		return std::byte(0x01);
	} else if (gender == Gender::MALE) {
		return std::byte(0x02);
	}
	throw ByteError(
		std::format("Invalid creature gender byte: {}", static_cast<unsigned int>(gender)));
}

std::byte CreatureSRI::serialize_specie(const CreatureSpecies &specie) {
	if (specie == CreatureSpecies::CRAB) {
		return std::byte(0x01);
	} else if (specie == CreatureSpecies::HIPPOPOTAMUS) {
		return std::byte(0x02);
	} else if (specie == CreatureSpecies::CROCODILE) {
		return std::byte(0x03);
	} else if (specie == CreatureSpecies::FISH) {
		return std::byte(0x04);
	}
	throw ByteError(std::format("Invalid creature specie: {}", static_cast<unsigned int>(specie)));
}

CreatureBytes CreatureSRI::serialize_creature(const Creature &creature) {
	CreatureBytes bytes;

	// Gender

	bytes[CreatureSRI::TO_GET_GENDER_OFFSET] = serialize_gender(creature.ontology.gender);

	// Specie

	bytes[CreatureSRI::TO_GET_SPECIE_OFFSET] = serialize_specie(creature.genome.core_genome.specie);

	// Position

	const auto vec = VecSRI::serialize_vec(creature.position);

	Deserializer::append_bytes(bytes, vec, CreatureSRI::TO_GET_POSITION_X_OFFSET);
	Deserializer::append_bytes(bytes, vec, CreatureSRI::TO_GET_POSITION_Y_OFFSET);
	Deserializer::append_bytes(bytes, vec, CreatureSRI::TO_GET_POSITION_Z_OFFSET);


	return bytes;
}