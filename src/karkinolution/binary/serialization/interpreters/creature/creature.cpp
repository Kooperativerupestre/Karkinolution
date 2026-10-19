#include "karkinolution/binary/serialization/interpreters/creature/creature.hpp"

#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/serialization/interpreters/creature/alias.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <format>
#include <limits>

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
	return Serializer::convert_uint8_t(static_cast<uint8_t>(specie));
}

std::vector<std::byte> CreatureSRI::serialize_name(const std::string &name) {
	if (name.size() > Layout::MAX_NAME_SIZE) {
		throw ByteError(std::format("The size of the name {} is bigger than allowed {}",
									name,
									Layout::MAX_NAME_SIZE));
	}

	std::vector<std::byte> bytes;

	const auto name_size = name.size();
	if (name_size > std::numeric_limits<uint8_t>::max()) {
		throw ByteError(std::format("The size of name {} exceeds the uint8_t max {}",
									name_size,
									std::numeric_limits<uint8_t>::max()));
	}
	bytes.push_back(Serializer::convert_uint8_t(static_cast<uint8_t>(name.size())));
	BinaryUtils::append_bytes(bytes, Serializer::convert_string(name));
	return bytes;
}

CreatureSRI::BytesArrays::Creature CreatureSRI::serialize_creature(const class Creature &creature) {
	CreatureSRI::BytesArrays::Creature bytes{};

	// Gender

	bytes[Layout::Gender::OFFSET] = serialize_gender(creature.ontology.gender);

	// Specie

	bytes[Layout::Specie::OFFSET] = serialize_specie(creature.genome.core_genome.specie);

	// Position

	const auto vec = VecSRI::serialize_vec(creature.position);

	BinaryUtils::append_bytes(bytes, vec, Layout::PositionX::OFFSET);

	// Name

	const auto name = serialize_name(creature.ontology.name);
	BinaryUtils::append_bytes(bytes, name, Layout::NameSize::OFFSET);

	// Id

	const auto id = Serializer::serialize<BaseIdType>(creature.ontology.id);
	BinaryUtils::append_bytes(bytes, id, Layout::Id::OFFSET);

	return bytes;
}

std::array<std::byte, 8> CreatureSRI::Request::make_see_creature(BaseIdType id) {
	return Serializer::serialize<BaseIdType>(id);
}