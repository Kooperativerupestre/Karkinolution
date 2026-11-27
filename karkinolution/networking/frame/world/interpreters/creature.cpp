#include "karkinolution/networking/frame/world/interpreters/creature.hpp"

#include "karkinolution/organism/entities/creature/ontology.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/errors.hpp"
#include "shared/binary/serializer.hpp"
#include "shared/core/id.hpp"

#include <cstdlib>
#include <format>
#include <limits>
#include <magic_enum/magic_enum.hpp>
#include <shared/binary/interpreters/vec.hpp>

// SRI implementations
std::byte CreatureSRI::serialize_gender(const Karkinolution::Organisms::Entities::Creature::Gender &gender) {
	return KShared::Binary::Serializer::convert_uint8_t(static_cast<uint8_t>(gender));
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

	// Size
	KShared::Binary::Utils::append_bytes(bytes,
										 UnitsSRI::serialize_size(creature.body.morphology.size),
										 Layout::Size::OFFSET);
	KShared::Binary::Utils::append_bytes(bytes,
										 UnitsSRI::serialize_mass(creature.body.morphology.mass),
										 Layout::Mass::OFFSET);

	// Life
	KShared::Binary::Utils::append_bytes(bytes,
										 StatsSRI::serialize_limited_value(creature.body.vital.life),
										 Layout::Life::OFFSET);

	KShared::Binary::Utils::append_bytes(bytes,
										 StatsSRI::serialize_normalized_value(creature.body.vital.health),
										 Layout::Health::OFFSET);

	// Energy
	KShared::Binary::Utils::append_bytes(bytes,
										 StatsSRI::serialize_limited_value(creature.body.metabolism.energy),
										 Layout::Energy::OFFSET);
	KShared::Binary::Utils::append_bytes(bytes,
										 StatsSRI::serialize_limited_value(creature.body.metabolism.reserved),
										 Layout::ReservedEnergy::OFFSET);

	// Age
	KShared::Binary::Utils::append_bytes(bytes,
										 KShared::Binary::Serializer::convert_float(creature.body.age.value),
										 Layout::Age::OFFSET);

	KShared::Binary::Utils::append_bytes(bytes,
										 KShared::Binary::Serializer::convert_uint32_t(creature.brain.intent().time),
										 Layout::IntentTime::OFFSET);
	KShared::Binary::Utils::append_bytes(
		bytes,
		KShared::Binary::Serializer::convert_uint8_t(static_cast<std::uint8_t>(creature.brain.intent().type)),
		Layout::IntentType::OFFSET);

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

Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::Creature
CreatureDSI::get_creature(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::Creature{
		.gender     = get_gender(payload, offset + CreatureSRI::Layout::Gender::OFFSET),
		.specie     = get_specie(payload, offset + CreatureSRI::Layout::Specie::OFFSET),
		.position   = VecDSI::get_vec(payload, offset + CreatureSRI::Layout::PositionX::OFFSET),
		.name       = get_name(payload, offset + CreatureSRI::Layout::NameSize::OFFSET),
		.id         = get_id(payload, offset + CreatureSRI::Layout::Id::OFFSET),
		.morphology = {.size = get_size(payload, offset + CreatureSRI::Layout::Size::OFFSET),
					   .mass = get_mass(payload, offset + CreatureSRI::Layout::Mass::OFFSET)},
		.vital      = {.life   = get_life(payload, offset + CreatureSRI::Layout::Life::OFFSET),
					   .health = get_health(payload, offset + CreatureSRI::Layout::Health::OFFSET)},
		.metabolism = {.energy = get_energy(payload, offset + CreatureSRI::Layout::Energy::OFFSET),
					   .reserved_energy =
						   get_reserved_energy(payload, offset + CreatureSRI::Layout::ReservedEnergy::OFFSET)},
		.brain      = {.intent = get_intent(payload, offset + CreatureSRI::Layout::IntentTime::OFFSET)},
		.age        = get_age(payload, offset + CreatureSRI::Layout::Age::OFFSET)};
}

KShared::Math::Units::Size CreatureDSI::get_size(const std::vector<std::byte> &payload, std::size_t offset) {
	return UnitsDSI::deserialize_size(payload, offset);
}

KShared::Math::Units::Mass CreatureDSI::get_mass(const std::vector<std::byte> &payload, std::size_t offset) {
	return UnitsDSI::deserialize_mass(payload, offset);
}

KShared::Binary::Interpreters::Models::Stats::StatValue<float>
CreatureDSI::get_life(const std::vector<std::byte> &payload, std::size_t offset) {
	return StatsDSI::get_stats<float>(payload, offset);
}

KShared::Binary::Interpreters::Models::Stats::StatValue<float>
CreatureDSI::get_health(const std::vector<std::byte> &payload, std::size_t offset) {
	return StatsDSI::get_stats<float>(payload, offset);
}

KShared::Binary::Interpreters::Models::Stats::StatValue<float>
CreatureDSI::get_energy(const std::vector<std::byte> &payload, std::size_t offset) {
	return StatsDSI::get_stats<float>(payload, offset);
}

KShared::Binary::Interpreters::Models::Stats::StatValue<float>
CreatureDSI::get_reserved_energy(const std::vector<std::byte> &payload, std::size_t offset) {
	return StatsDSI::get_stats<float>(payload, offset);
}

float CreatureDSI::get_age(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Binary::Deserializer::read_float(payload, offset);
}

Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::IntentData
CreatureDSI::get_intent(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto raw_type = KShared::Binary::Deserializer::read_uint8_t(payload,
																	  offset + CreatureSRI::Layout::IntentType::OFFSET
																		  - CreatureSRI::Layout::IntentTime::OFFSET);
	const auto type     = Karkinolution::Organisms::Entities::Creature::Brain::Intents::IntentTypes(raw_type);
	if (!magic_enum::enum_contains(type)) {
		throw KShared::Binary::ByteError(std::format("Invalid creature intent type: {}", raw_type));
	}
	return {.time = KShared::Binary::Deserializer::read_uint32_t(payload, offset), .type = type};
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
