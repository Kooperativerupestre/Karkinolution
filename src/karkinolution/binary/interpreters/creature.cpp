#include "karkinolution/binary/interpreters/creature.hpp"

#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <format>
#include <karkinolution/binary/interpreters/vec.hpp>
#include <limits>
#include <magic_enum/magic_enum.hpp>

// SRI implementations
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

// DSI implementations
Gender CreatureDSI::get_gender(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto raw_gender = payload[CreatureSRI::Layout::Gender::OFFSET + offset];
	const auto gender     = Gender(raw_gender);
	if (!magic_enum::enum_contains(gender)) {
		throw ByteError(std::format("Invalid gender: {}", BinaryUtils::byte_to_int(raw_gender)));
	}
	return gender;
}

CreatureSpecies CreatureDSI::get_specie(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto raw_specie = payload[CreatureSRI::Layout::Specie::OFFSET + offset];
	const auto specie     = CreatureSpecies(raw_specie);
	if (!magic_enum::enum_contains(specie)) {
		throw ByteError(std::format("Invalid speciie: {}", BinaryUtils::byte_to_int(raw_specie)));
	}
	return specie;
}

Vec3 CreatureDSI::get_position(const std::vector<std::byte> &payload, std::size_t offset) {
	return VecDSI::get_vec(payload, offset + CreatureSRI::Layout::PositionX::OFFSET);
}

CreatureDSI::Models::Creature CreatureDSI::get_creature(const std::vector<std::byte> &payload,
														std::size_t                   offset) {
	return Models::Creature{.gender   = get_gender(payload, offset),
							.specie   = get_specie(payload, offset),
							.position = get_position(payload, offset),
							.name     = get_name(payload, offset),
							.id       = get_id(payload, offset)};
}

uint8_t CreatureDSI::get_name_size(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::read_uint8_t(payload, offset + CreatureSRI::Layout::NameSize::OFFSET);
}

std::string CreatureDSI::get_name(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto name_size = get_name_size(payload, offset);
	return Deserializer::read_string(payload,
									 offset + CreatureSRI::Layout::Name::OFFSET,
									 name_size);
}

BaseIdType CreatureDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::deserialize<BaseIdType>(payload, offset + CreatureSRI::Layout::Id::OFFSET);
}

BaseIdType
CreatureDSI::Request::interpret_like_get_creature(const std::vector<std::byte> &payload) {
	return BaseIdType{Deserializer::deserialize<BaseIdType>(payload, 0)};
}
