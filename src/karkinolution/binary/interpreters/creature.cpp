#include "karkinolution/binary/interpreters/creature.hpp"

#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <cstdlib>
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

std::array<std::byte, CreatureSRI::Request::Layout::CreateCreature::TOTAL_SIZE>
CreatureSRI::Request::make_create_creature(std::optional<Gender>             gender,
										   std::optional<CreatureSpecies>    specie,
										   const std::optional<Vec3>        &position,
										   const std::optional<std::string> &name) {
	if (name) {
		if (name->size() > CreatureSRI::Layout::MAX_NAME_SIZE) {
			throw SimulationError(std::format("The max size of name is {}, received {}",
											  CreatureSRI::Layout::MAX_NAME_SIZE,
											  name->size()));
		}
	}


	std::array<std::byte, CreatureSRI::Request::Layout::CreateCreature::TOTAL_SIZE> bytes;


	if (!gender) {
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_bool(false),
								  Layout::CreateCreature::HasGender::OFFSET);
	} else {
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_bool(true),
								  Layout::CreateCreature::HasGender::OFFSET);
		BinaryUtils::append_bytes(bytes,
								  CreatureSRI::serialize_gender(*gender),
								  Layout::CreateCreature::Gender::OFFSET);
	}

	if (!specie) {
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_bool(false),
								  Layout::CreateCreature::HasSpecies::OFFSET);

	} else {
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_bool(true),
								  Layout::CreateCreature::HasSpecies::OFFSET);
		BinaryUtils::append_bytes(bytes,
								  CreatureSRI::serialize_specie(*specie),
								  Layout::CreateCreature::Species::OFFSET);
	}

	if (!position) {
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_bool(false),
								  Layout::CreateCreature::HasPosition::OFFSET);

	} else {
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_bool(true),
								  Layout::CreateCreature::HasPosition::OFFSET);
		BinaryUtils::append_bytes(bytes,
								  VecSRI::serialize_vec(*position),
								  Layout::CreateCreature::Position::OFFSET);
	}

	if (!name) {
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_bool(false),
								  Layout::CreateCreature::HasName::OFFSET);
	} else {
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_bool(false),
								  Layout::CreateCreature::HasName::OFFSET);
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_uint8_t(static_cast<uint8_t>(name->size())),
								  Layout::CreateCreature::NameSize::OFFSET);
		BinaryUtils::append_bytes(bytes,
								  Serializer::convert_string(*name),
								  Layout::CreateCreature::Name::OFFSET);
	}
	return bytes;
}

// DSI implementations
Gender CreatureDSI::get_gender(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto raw_gender = payload[offset];
	const auto gender     = Gender(raw_gender);
	if (!magic_enum::enum_contains(gender)) {
		throw ByteError(std::format("Invalid gender: {}", BinaryUtils::byte_to_int(raw_gender)));
	}
	return gender;
}

CreatureSpecies CreatureDSI::get_specie(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto raw_specie = payload[offset];
	const auto specie     = CreatureSpecies(raw_specie);
	if (!magic_enum::enum_contains(specie)) {
		throw ByteError(std::format("Invalid speciie: {}", BinaryUtils::byte_to_int(raw_specie)));
	}
	return specie;
}

Karkinolution::Binary::Interpreters::Models::Creature::Creature
CreatureDSI::get_creature(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Binary::Interpreters::Models::Creature::Creature{
		.gender   = get_gender(payload, offset + CreatureSRI::Layout::Gender::OFFSET),
		.specie   = get_specie(payload, offset + CreatureSRI::Layout::Specie::OFFSET),
		.position = VecDSI::get_vec(payload, offset + CreatureSRI::Layout::PositionX::OFFSET),
		.name     = get_name(payload, offset + CreatureSRI::Layout::NameSize::OFFSET),
		.id       = get_id(payload, offset + CreatureSRI::Layout::Id::OFFSET)};
}

uint8_t CreatureDSI::get_name_size(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::read_uint8_t(payload, offset);
}

std::string CreatureDSI::get_name(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto name_size = get_name_size(payload, offset);
	return Deserializer::read_string(payload, offset + sizeof(name_size), name_size);
}

BaseIdType CreatureDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::deserialize<BaseIdType>(payload, offset);
}

BaseIdType
CreatureDSI::Request::interpret_like_get_creature(const std::vector<std::byte> &payload) {
	return BaseIdType{Deserializer::deserialize<BaseIdType>(payload, 0)};
}

CreatureBlueprint
CreatureDSI::Request::interpret_like_create_creature(const std::vector<std::byte> &payload) {
	std::optional<Gender>          gender   = std::nullopt;
	std::optional<CreatureSpecies> species  = std::nullopt;
	std::optional<Vec3>            position = std::nullopt;
	std::optional<std::string>     name     = std::nullopt;

	const auto has_gender = Deserializer::deserialize<bool>(
		payload,
		CreatureSRI::Request::Layout::CreateCreature::HasGender::OFFSET);

	const auto has_specie = Deserializer::deserialize<bool>(
		payload,
		CreatureSRI::Request::Layout::CreateCreature::HasSpecies::OFFSET);

	const auto has_position = Deserializer::deserialize<bool>(
		payload,
		CreatureSRI::Request::Layout::CreateCreature::HasPosition::OFFSET);

	const auto has_name = Deserializer::deserialize<bool>(
		payload,
		CreatureSRI::Request::Layout::CreateCreature::HasName::OFFSET);

	if (has_gender) {
		gender =
			CreatureDSI::get_gender(payload,
									CreatureSRI::Request::Layout::CreateCreature::Gender::OFFSET);
	}

	if (has_specie) {
		species =
			CreatureDSI::get_specie(payload,
									CreatureSRI::Request::Layout::CreateCreature::Species::OFFSET);
	}

	if (has_position) {
		position = VecDSI::get_vec(payload,
								   CreatureSRI::Request::Layout::CreateCreature::Position::OFFSET);
	}
	if (has_name) {
		name =
			CreatureDSI::get_name(payload,
								  CreatureSRI::Request::Layout::CreateCreature::NameSize::OFFSET);
	}
	return CreatureBlueprint{.specie   = species,
							 .name     = name,
							 .gender   = gender,
							 .position = position};
}