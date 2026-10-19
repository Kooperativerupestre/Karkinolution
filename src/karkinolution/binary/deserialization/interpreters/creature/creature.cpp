
#include "karkinolution/binary/serialization/interpreters/creature/creature.hpp"

#include "karkinolution/binary/deserialization/interpreters/creature/creature.hpp"
#include "karkinolution/binary/deserialization/interpreters/math/vec/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/creature/alias.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"

#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature/alias.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <magic_enum/magic_enum.hpp>

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
