#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/deserialization/interpreters/properties/properties.hpp"
#include "karkinolution/binary/deserialization/interpreters/territory/soil/soil.hpp"
#include "karkinolution/binary/serialization/interpreters/math/vec/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/territory/soil/soil.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/math/geometry/models.hpp"
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "karkinolution/utils/k_random.hpp"

#include <gtest/gtest.h>
#include <karkinolution/binary/deserialization/interpreters/corpse/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/unit/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/properties/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/territory/soil/alias.hpp>
#include <karkinolution/binary/deserialization/interpreters/territory/territory/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/corpse/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/creature/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/geometry/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/unit/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/properties/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/territory/soil/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/territory/territory/alias.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/test/utils.hpp>
#include <limits>

// Properties: GenericProperty Roundtrip

TEST(PropertiesSerializeDeserializeRoundtrip, GenericProperty_Roundtrip) {
	GenericProperty p_swim{Properties::Capabilities::Move::SWIMM};
	const auto      s_p_swim = PropertiesSRI::serialize(p_swim);

	GenericProperty p_walk{Properties::Capabilities::Move::WALK};
	const auto      s_p_walk = PropertiesSRI::serialize(p_walk);

	EXPECT_EQ(PropertiesDSI::deserialize(s_p_swim, 0), p_swim);
	EXPECT_EQ(PropertiesDSI::deserialize(s_p_walk, 0), p_walk);
}

// Properties: Stream With Offsets

TEST(PropertiesSerializeDeserializeRoundtrip, StreamWithOffsets) {
	std::vector<std::byte> stream = {std::byte{0xAA}, std::byte{0xBB}};

	const size_t offset_swimm = stream.size();
	Deserializer::append_bytes(
		stream,
		PropertiesSRI::serialize(GenericProperty(Properties::Capabilities::Move::SWIMM)));

	const size_t offset_walk = stream.size();
	Deserializer::append_bytes(
		stream,
		PropertiesSRI::serialize(GenericProperty(Properties::Capabilities::Move::WALK)));

	stream.push_back(std::byte{0xCC});

	const auto deserialized_swimm = PropertiesDSI::deserialize(stream, offset_swimm);
	const auto deserialized_walk  = PropertiesDSI::deserialize(stream, offset_walk);

	EXPECT_EQ(deserialized_swimm, GenericProperty(Properties::Capabilities::Move::SWIMM));
	EXPECT_EQ(deserialized_walk, GenericProperty(Properties::Capabilities::Move::WALK));
}

// Properties: Error Handling

TEST(PropertiesSerializeDeserializeRoundtrip, ErrorHandling) {
	std::vector<std::byte> bytes;

	std::size_t invalid_properties_count   = 0;
	std::size_t minimum_invalid_properties = 10;

	std::size_t repeats_count = 0;
	std::size_t max_repeats   = 1000;

	while (invalid_properties_count < minimum_invalid_properties && repeats_count < max_repeats) {
		repeats_count++;

		for (size_t i = 0; i < 5; i++) {
			const auto kind = static_cast<std::byte>(RandomGenerators::generate<uint8_t>());
			const auto type = static_cast<std::byte>(RandomGenerators::generate<uint8_t>());

			const auto kind_offset = bytes.size();

			std::optional<GenericProperty> is_valid =
				GenericProperty::fromRaw(Properties::Kind(static_cast<uint8_t>(kind)),
										 Properties::Type(static_cast<uint8_t>(type)));

			if (is_valid) {
				continue;
			}

			invalid_properties_count++;
			bytes.push_back(kind);
			bytes.push_back(type);

			EXPECT_THROW(PropertiesDSI::deserialize(bytes, kind_offset), ByteError);
		}
	}

	ASSERT_GE(invalid_properties_count, minimum_invalid_properties);
}

// Terrain Soil: Full Serialization

TEST(SoilSerializeDeserializeRoundtrip, FullSoilSerialization) {
	const auto       damage_value  = std::numeric_limits<float>::max();
	const auto       movement_cost = 0.001f;
	const BaseIdType id            = 248539453;
	const Radius     radius{24};

	const Vec3 position = {0,
						   std::numeric_limits<double>::max(),
						   std::numeric_limits<double>::min()};

	const std::vector<SoilProperties> properties{SoilProperties::DANGEROUS};

	const std::vector<GenericProperty> required_properties = {
		GenericProperty(Properties::Capabilities::Move::SWIMM),
		GenericProperty(Properties::Capabilities::Move::WALK)};

	SoilPiece soil{.type                  = SoilTypes::DIRT,
				   .properties            = properties,
				   .required_capabilities = required_properties,
				   .components            = {},
				   .radius                = radius,
				   .position              = position,
				   .id                    = id};

	soil.components.add(SoilPieceComponents::Damage{damage_value});
	soil.components.add(SoilPieceComponents::MovementCost{movement_cost});

	const auto                   serialized = SoilSRI::serialize_soil(soil);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const SoilDSI::Models::Soil copy = SoilDSI::get_soil(bytes, 0);

	EXPECT_EQ(copy.id, id);
	EXPECT_EQ(copy.type, soil.type);
	EXPECT_EQ(copy.properties, properties);
	EXPECT_EQ(copy.required_properties, required_properties);

	EXPECT_DOUBLE_EQ(copy.radius.value, radius.value);

	EXPECT_DOUBLE_EQ(copy.position.x, position.x);
	EXPECT_DOUBLE_EQ(copy.position.y, position.y);
	EXPECT_DOUBLE_EQ(copy.position.z, position.z);

	EXPECT_FLOAT_EQ(copy.damage_component.value().value, damage_value);
	EXPECT_FLOAT_EQ(copy.movement_cost_component.value().cost, movement_cost);
}

// Corpse: Full Serialization

TEST(CorpseSerializeDeserializeRoundtrip, FullCorpseSerialization) {
	const BaseIdType id = 248539453;

	const OrganismStats::Body::RawMeat raw_meat{std::numeric_limits<float>::max()};

	const Vec3 position = {0.0,
						   std::numeric_limits<double>::max(),
						   std::numeric_limits<double>::min()};

	const Size size{12.5, 24.0, 36.5};

	Corpse corpse{size, position, raw_meat, id};

	const auto                   serialized = CorpseSRI::serialize_corpse(corpse);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const auto copy = CorpseDSI::get_corpse(bytes, 0);

	EXPECT_EQ(copy.id, id);
	EXPECT_FLOAT_EQ(copy.raw_meat.value, raw_meat.value);

	EXPECT_DOUBLE_EQ(copy.position.x, position.x);
	EXPECT_DOUBLE_EQ(copy.position.y, position.y);
	EXPECT_DOUBLE_EQ(copy.position.z, position.z);

	EXPECT_DOUBLE_EQ(copy.size.lateral.value, size.lateral.value);
	EXPECT_DOUBLE_EQ(copy.size.height.value, size.height.value);
	EXPECT_DOUBLE_EQ(copy.size.depth.value, size.depth.value);
}

// Corpse: Request DSI

TEST(CorpseSerializeDeserializeRoundtrip, RequestDSI) {
	constexpr BaseIdType corpse_id = 248539453;

	const auto                   bytes = Serializer::serialize<BaseIdType>(corpse_id);
	const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

	const auto interpreted_id = CorpseDSI::Request::interpret_like_see_corpse(buffer);

	EXPECT_EQ(interpreted_id, corpse_id);
}

// Territory: Size Roundtrip

TEST(TerritorySerializeDeserializeRoundtrip, FullFields) {
	const Size size{12.5, 24.0, 36.5};

	const auto                   serialized = UnitsSRI::serialize_size(size);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const auto size_copy = UnitsDSI::deserialize_size(bytes, 0);

	EXPECT_DOUBLE_EQ(size_copy.lateral.value, size.lateral.value);
	EXPECT_DOUBLE_EQ(size_copy.height.value, size.height.value);
	EXPECT_DOUBLE_EQ(size_copy.depth.value, size.depth.value);
}

// Territory: Full Serialization

TEST(TerritorySerializeDeserializeRoundtrip, FullTerritorySerialization) {
	const Size size{12.5, 24.0, 36.5};

	Territory territory{size};

	const auto                   serialized = TerritorySRI::serialize_territory(territory);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const auto copy = TerritoryDSI::deserialize_territory(bytes);

	EXPECT_DOUBLE_EQ(copy.size.lateral.value, size.lateral.value);
	EXPECT_DOUBLE_EQ(copy.size.height.value, size.height.value);
	EXPECT_DOUBLE_EQ(copy.size.depth.value, size.depth.value);
}

// Creature: Full Serialization

TEST(CreatureSerializeDeserializeRoundtrip, FullCreatureSerialization) {
	const Gender          gender = Gender::FEMALE;
	const CreatureSpecies specie = CreatureSpecies::FISH;

	const Vec3 position{12.5, 24.0, 36.5};

	const std::string name = "Karkinolution";

	const auto creature = CreatureConstructor::from_blueprint(
		CreatureBlueprint{.specie = specie, .name = name, .gender = gender, .position = position});

	const auto                   serialized = CreatureSRI::serialize_creature(creature);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const auto copy = CreatureDSI::get_creature(bytes, 0);

	EXPECT_EQ(copy.gender, gender);
	EXPECT_EQ(copy.specie, specie);

	EXPECT_DOUBLE_EQ(copy.position.x, position.x);
	EXPECT_DOUBLE_EQ(copy.position.y, position.y);
	EXPECT_DOUBLE_EQ(copy.position.z, position.z);

	EXPECT_EQ(copy.name, name);
}