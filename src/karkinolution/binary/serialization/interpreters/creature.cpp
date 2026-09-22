#include "karkinolution/binary/serialization/interpreters/creature.hpp"

#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"


using CreatureSRI::CreatureBytes;

using CreatureSRI::CreatureBytes;

CreatureBytes CreatureSRI::serialize_creature(const Creature &creature) {
	CreatureBytes bytes;

	// Gender

	if (creature.ontology.gender == Gender::FEMALE) {
		bytes[0] = std::byte(0x01);
	} else {
		bytes[0] = std::byte(0x02);
	}

	// Specie

	if (creature.genome.core_genome.specie == CreatureSpecies::CRAB) {
		bytes[1] = std::byte(0x01);
	} else if (creature.genome.core_genome.specie == CreatureSpecies::HIPPOPOTAMUS) {
		bytes[1] = std::byte(0x02);
	} else if (creature.genome.core_genome.specie == CreatureSpecies::CROCODILE) {
		bytes[1] = std::byte(0x03);
	} else {
		bytes[1] = std::byte(0x04);
	}

	// Position

	const auto vec = VecSRI::serialize_vec(creature.position);

	Deserializer::append_bytes(bytes, vec, CreatureSRI::TO_GET_POSITION_X_OFFSET);
	Deserializer::append_bytes(bytes, vec, CreatureSRI::TO_GET_POSITION_Y_OFFSET);
	Deserializer::append_bytes(bytes, vec, CreatureSRI::TO_GET_POSITION_Z_OFFSET);


	return bytes;
}