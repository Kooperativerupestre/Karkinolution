#include "karkinolution/binary/serialization/interpreters/creature.hpp"

#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <format>

using CreatureSRI::CreatureBytes;

using CreatureSRI::NameBytes;

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

NameBytes CreatureSRI::serialize_name(const std::string &name) {
	if (name.size() > NAME_BYTES) {
		throw ByteError(
			std::format("The size of the name {} is bigger than allowed {}", name, NAME_BYTES));
	}

	NameBytes bytes;

	const auto serialized_name = Serializer::convert_string(name);

	Deserializer::append_bytes(bytes, serialized_name, 0);
	return bytes;
}

CreatureBytes CreatureSRI::serialize_creature(const Creature &creature) {
	CreatureBytes bytes;

	// Gender

	bytes[CreatureSRI::TO_GET_GENDER_OFFSET] = serialize_gender(creature.ontology.gender);

	// Specie

	bytes[CreatureSRI::TO_GET_SPECIE_OFFSET] = serialize_specie(creature.genome.core_genome.specie);

	// Position

	const auto vec = VecSRI::serialize_vec(creature.position);

	Deserializer::append_bytes(bytes, vec, CreatureSRI::TO_GET_POSITION_OFFSET);

	// Name

	const auto name = serialize_name(creature.ontology.name);
	Deserializer::append_bytes(bytes, name, CreatureSRI::TO_GET_NAME_OFFSET);

	return bytes;
}