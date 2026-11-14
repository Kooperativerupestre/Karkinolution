#include "karkinolution/binary/interpreters/creature.hpp"

#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/errors.hpp"
#include "shared/binary/serializer.hpp"
#include "shared/core/error.hpp"
#include "shared/core/id.hpp"

#include <cstdlib>
#include <format>
#include <limits>
#include <magic_enum/magic_enum.hpp>
#include <shared/binary/interpreters/vec.hpp>

// SRI implementations
std::byte CreatureSRI::serialize_gender(const Karkinolution::Organisms::Entities::Creature::Gender &gender) {
	if (gender == Karkinolution::Organisms::Entities::Creature::Gender::FEMALE) {
		return std::byte(0x01);
	} else if (gender == Karkinolution::Organisms::Entities::Creature::Gender::MALE) {
		return std::byte(0x02);
	}
	throw KShared::Binary::ByteError(
		std::format("Invalid creature gender byte: {}", static_cast<unsigned int>(gender)));
}

std::byte CreatureSRI::serialize_specie(const Karkinolution::Organisms::Entities::Creature::CreatureSpecies &specie) {
	return KShared::Binary::Serializer::convert_uint8_t(static_cast<uint8_t>(specie));
}

std::vector<std::byte> CreatureSRI::serialize_name(const std::string &name) {
	if (name.size() > Layout::MAX_NAME_SIZE) {
		throw KShared::Binary::ByteError(
			std::format("The size of the name {} is bigger than allowed {}", name, Layout::MAX_NAME_SIZE));
	}

	std::vector<std::byte> bytes;

	const auto name_size = name.size();
	if (name_size > std::numeric_limits<uint8_t>::max()) {
		throw KShared::Binary::ByteError(std::format("The size of name {} exceeds the uint8_t max {}",
													 name_size,
													 std::numeric_limits<uint8_t>::max()));
	}
	bytes.push_back(KShared::Binary::Serializer::convert_uint8_t(static_cast<uint8_t>(name.size())));
	KShared::Binary::Utils::append_bytes(bytes, KShared::Binary::Serializer::convert_string(name));
	return bytes;
}

CreatureSRI::BytesArrays::Creature
CreatureSRI::serialize_creature(const Karkinolution::Organisms::Entities::Creature::Creature &creature) {
	CreatureSRI::BytesArrays::Creature bytes{};

	// Gender

	bytes[Layout::Gender::OFFSET] = serialize_gender(creature.ontology.gender);

	// Specie

	bytes[Layout::Specie::OFFSET] = serialize_specie(creature.genome.core_genome.specie);

	// Position

	const auto vec = VecSRI::serialize_vec(creature.position);

	KShared::Binary::Utils::append_bytes(bytes, vec, Layout::PositionX::OFFSET);

	// Name

	const auto name = serialize_name(creature.ontology.name);
	KShared::Binary::Utils::append_bytes(bytes, name, Layout::NameSize::OFFSET);

	// Id

	const auto id = KShared::Binary::Serializer::serialize<KShared::Core::Id::BaseIdType>(creature.ontology.id);
	KShared::Binary::Utils::append_bytes(bytes, id, Layout::Id::OFFSET);

	return bytes;
}

std::array<std::byte, 8> CreatureSRI::Request::make_see_creature(KShared::Core::Id::BaseIdType id) {
	return KShared::Binary::Serializer::serialize<KShared::Core::Id::BaseIdType>(id);
}

std::array<std::byte, CreatureSRI::Request::Layout::CreateCreature::TOTAL_SIZE>
CreatureSRI::Request::make_create_creature(
	std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender,
	std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> specie,
	const std::optional<KShared::Math::Vec::Vec2>                               &position,
	const std::optional<std::string>                                            &name) {
	if (name) {
		if (name->size() > CreatureSRI::Layout::MAX_NAME_SIZE) {
			throw KShared::Errors::SimulationError(std::format("The max size of name is {}, received {}",
															   CreatureSRI::Layout::MAX_NAME_SIZE,
															   name->size()));
		}
	}


	std::array<std::byte, CreatureSRI::Request::Layout::CreateCreature::TOTAL_SIZE> bytes;


	if (!gender) {
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_bool(false),
											 Layout::CreateCreature::HasGender::OFFSET);
	} else {
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_bool(true),
											 Layout::CreateCreature::HasGender::OFFSET);
		KShared::Binary::Utils::append_bytes(bytes,
											 CreatureSRI::serialize_gender(*gender),
											 Layout::CreateCreature::Gender::OFFSET);
	}

	if (!specie) {
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_bool(false),
											 Layout::CreateCreature::HasSpecies::OFFSET);

	} else {
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_bool(true),
											 Layout::CreateCreature::HasSpecies::OFFSET);
		KShared::Binary::Utils::append_bytes(bytes,
											 CreatureSRI::serialize_specie(*specie),
											 Layout::CreateCreature::Species::OFFSET);
	}

	if (!position) {
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_bool(false),
											 Layout::CreateCreature::HasPosition::OFFSET);

	} else {
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_bool(true),
											 Layout::CreateCreature::HasPosition::OFFSET);
		KShared::Binary::Utils::append_bytes(bytes,
											 VecSRI::serialize_vec(*position),
											 Layout::CreateCreature::Position::OFFSET);
	}

	if (!name) {
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_bool(false),
											 Layout::CreateCreature::HasName::OFFSET);
	} else {
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_bool(false),
											 Layout::CreateCreature::HasName::OFFSET);
		KShared::Binary::Utils::append_bytes(
			bytes,
			KShared::Binary::Serializer::convert_uint8_t(static_cast<uint8_t>(name->size())),
			Layout::CreateCreature::NameSize::OFFSET);
		KShared::Binary::Utils::append_bytes(bytes,
											 KShared::Binary::Serializer::convert_string(*name),
											 Layout::CreateCreature::Name::OFFSET);
	}
	return bytes;
}

// DSI implementations
Karkinolution::Organisms::Entities::Creature::Gender CreatureDSI::get_gender(const std::vector<std::byte> &payload,
																			 std::size_t                   offset) {
	const auto raw_gender = payload[offset];
	const auto gender     = Karkinolution::Organisms::Entities::Creature::Gender(raw_gender);
	if (!magic_enum::enum_contains(gender)) {
		throw KShared::Binary::ByteError(
			std::format("Invalid gender: {}", KShared::Binary::Utils::byte_to_int(raw_gender)));
	}
	return gender;
}

Karkinolution::Organisms::Entities::Creature::CreatureSpecies
CreatureDSI::get_specie(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto raw_specie = payload[offset];
	const auto specie     = Karkinolution::Organisms::Entities::Creature::CreatureSpecies(raw_specie);
	if (!magic_enum::enum_contains(specie)) {
		throw KShared::Binary::ByteError(
			std::format("Invalid speciie: {}", KShared::Binary::Utils::byte_to_int(raw_specie)));
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
	return KShared::Binary::Deserializer::read_uint8_t(payload, offset);
}

std::string CreatureDSI::get_name(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto name_size = get_name_size(payload, offset);
	return KShared::Binary::Deserializer::read_string(payload, offset + sizeof(name_size), name_size);
}

KShared::Core::Id::BaseIdType CreatureDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Binary::Deserializer::deserialize<KShared::Core::Id::BaseIdType>(payload, offset);
}

KShared::Core::Id::BaseIdType CreatureDSI::Request::interpret_like_get_creature(const std::vector<std::byte> &payload) {
	return KShared::Core::Id::BaseIdType{
		KShared::Binary::Deserializer::deserialize<KShared::Core::Id::BaseIdType>(payload, 0)};
}

Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint
CreatureDSI::Request::interpret_like_create_creature(const std::vector<std::byte> &payload) {
	std::optional<Karkinolution::Organisms::Entities::Creature::Gender>          gender   = std::nullopt;
	std::optional<Karkinolution::Organisms::Entities::Creature::CreatureSpecies> species  = std::nullopt;
	std::optional<KShared::Math::Vec::Vec2>                                      position = std::nullopt;
	std::optional<std::string>                                                   name     = std::nullopt;

	const auto has_gender = KShared::Binary::Deserializer::deserialize<bool>(
		payload,
		CreatureSRI::Request::Layout::CreateCreature::HasGender::OFFSET);

	const auto has_specie = KShared::Binary::Deserializer::deserialize<bool>(
		payload,
		CreatureSRI::Request::Layout::CreateCreature::HasSpecies::OFFSET);

	const auto has_position = KShared::Binary::Deserializer::deserialize<bool>(
		payload,
		CreatureSRI::Request::Layout::CreateCreature::HasPosition::OFFSET);

	const auto has_name =
		KShared::Binary::Deserializer::deserialize<bool>(payload,
														 CreatureSRI::Request::Layout::CreateCreature::HasName::OFFSET);

	if (has_gender) {
		gender = CreatureDSI::get_gender(payload, CreatureSRI::Request::Layout::CreateCreature::Gender::OFFSET);
	}

	if (has_specie) {
		species = CreatureDSI::get_specie(payload, CreatureSRI::Request::Layout::CreateCreature::Species::OFFSET);
	}

	if (has_position) {
		position = VecDSI::get_vec(payload, CreatureSRI::Request::Layout::CreateCreature::Position::OFFSET);
	}
	if (has_name) {
		name = CreatureDSI::get_name(payload, CreatureSRI::Request::Layout::CreateCreature::NameSize::OFFSET);
	}
	return Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint{.specie   = species,
																						.name     = name,
																						.gender   = gender,
																						.position = position};
}