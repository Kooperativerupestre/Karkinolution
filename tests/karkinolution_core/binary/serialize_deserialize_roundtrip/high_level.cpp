
#include "karkinolution/organism/entities/creature/constructor.hpp"
#include "karkinolution/organism/entities/genetics/genetic.hpp"
#include "shared/core/error.hpp"
#include "shared/core/id.hpp"
#include "shared/math/geometry/models.hpp"

#include <gtest/gtest.h>
#include <karkinolution/networking/frame/world/interpreters/corpse.hpp>
#include <karkinolution/networking/frame/world/interpreters/creature.hpp>
#include <karkinolution/networking/frame/world/interpreters/models/soil.hpp>
#include <karkinolution/networking/frame/world/interpreters/properties.hpp>
#include <karkinolution/networking/frame/world/interpreters/soil.hpp>
#include <karkinolution/networking/frame/world/interpreters/territory.hpp>
#include <karkinolution/networking/frame/world/message_interpreters/corpse.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>
#include <karkinolution/terrain/soil/soil.hpp>
#include <limits>
#include <shared/binary/interpreters/geometry.hpp>
#include <shared/binary/interpreters/unit.hpp>
#include <shared/binary/interpreters/vec.hpp>
#include <shared/functional/k_random.hpp>

// Properties: GenericProperty Roundtrip

TEST(PropertiesSerializeDeserializeRoundtrip, GenericProperty_Roundtrip) {
	Karkinolution::Organisms::Entities::GenericProperty p_swim{
		Karkinolution::Organisms::Entities::Properties::Capabilities::Move::SWIMM};
	const auto s_p_swim = PropertiesSRI::serialize(p_swim);

	Karkinolution::Organisms::Entities::GenericProperty p_walk{
		Karkinolution::Organisms::Entities::Properties::Capabilities::Move::WALK};
	const auto s_p_walk = PropertiesSRI::serialize(p_walk);

	EXPECT_EQ(PropertiesDSI::deserialize(s_p_swim, 0), p_swim);
	EXPECT_EQ(PropertiesDSI::deserialize(s_p_walk, 0), p_walk);
}

// Properties: Stream With Offsets

TEST(PropertiesSerializeDeserializeRoundtrip, StreamWithOffsets) {
	std::vector<std::byte> stream = {std::byte{0xAA}, std::byte{0xBB}};

	const size_t offset_swimm = stream.size();
	KShared::Binary::Utils::append_bytes(
		stream,
		PropertiesSRI::serialize(Karkinolution::Organisms::Entities::GenericProperty(
			Karkinolution::Organisms::Entities::Properties::Capabilities::Move::SWIMM)));

	const size_t offset_walk = stream.size();
	KShared::Binary::Utils::append_bytes(
		stream,
		PropertiesSRI::serialize(Karkinolution::Organisms::Entities::GenericProperty(
			Karkinolution::Organisms::Entities::Properties::Capabilities::Move::WALK)));

	stream.push_back(std::byte{0xCC});

	const auto deserialized_swimm = PropertiesDSI::deserialize(stream, offset_swimm);
	const auto deserialized_walk  = PropertiesDSI::deserialize(stream, offset_walk);

	EXPECT_EQ(deserialized_swimm,
			  Karkinolution::Organisms::Entities::GenericProperty(
				  Karkinolution::Organisms::Entities::Properties::Capabilities::Move::SWIMM));
	EXPECT_EQ(deserialized_walk,
			  Karkinolution::Organisms::Entities::GenericProperty(
				  Karkinolution::Organisms::Entities::Properties::Capabilities::Move::WALK));
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
			const auto kind =
				static_cast<std::byte>(KShared::Functional::KRandom::RandomGenerators::generate<uint8_t>());
			const auto type =
				static_cast<std::byte>(KShared::Functional::KRandom::RandomGenerators::generate<uint8_t>());

			const auto kind_offset = bytes.size();

			std::optional<Karkinolution::Organisms::Entities::GenericProperty> is_valid =
				Karkinolution::Organisms::Entities::GenericProperty::fromRaw(
					Karkinolution::Organisms::Entities::Properties::Kind(static_cast<uint8_t>(kind)),
					Karkinolution::Organisms::Entities::Properties::Type(static_cast<uint8_t>(type)));

			if (is_valid) {
				continue;
			}

			invalid_properties_count++;
			bytes.push_back(kind);
			bytes.push_back(type);

			EXPECT_THROW(PropertiesDSI::deserialize(bytes, kind_offset), KShared::Binary::ByteError);
		}
	}

	ASSERT_GE(invalid_properties_count, minimum_invalid_properties);
}

// Terrain Soil: Full Serialization

TEST(SoilSerializeDeserializeRoundtrip, FullSoilSerialization) {
	const auto                            damage_value  = std::numeric_limits<float>::max();
	const auto                            movement_cost = 0.001f;
	const KShared::Core::Id::BaseIdType   id            = 248539453;
	const KShared::Math::Geometry::Radius radius{24};

	const KShared::Math::Vec::Vec2 position = {0, std::numeric_limits<double>::max()};

	const std::vector<Karkinolution::Terrain::Soil::SoilProperties> properties{
		Karkinolution::Terrain::Soil::SoilProperties::DANGEROUS};

	const std::vector<Karkinolution::Organisms::Entities::GenericProperty> required_properties = {
		Karkinolution::Organisms::Entities::GenericProperty(
			Karkinolution::Organisms::Entities::Properties::Capabilities::Move::SWIMM),
		Karkinolution::Organisms::Entities::GenericProperty(
			Karkinolution::Organisms::Entities::Properties::Capabilities::Move::WALK)};

	Karkinolution::Terrain::Soil::SoilPiece soil{.type                  = Karkinolution::Terrain::Soil::SoilTypes::DIRT,
												 .properties            = properties,
												 .required_capabilities = required_properties,
												 .components            = {},
												 .radius                = radius,
												 .position              = position,
												 .id                    = id};

	soil.components.add(Karkinolution::Terrain::Soil::SoilPieceComponents::Damage{damage_value});
	soil.components.add(Karkinolution::Terrain::Soil::SoilPieceComponents::MovementCost{movement_cost});

	const auto                   serialized = SoilSRI::serialize_soil(soil);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const Karkinolution::Networking::Frames::World::Interpreters::Models::Soil::Soil copy = SoilDSI::get_soil(bytes, 0);

	EXPECT_EQ(copy.id, id);
	EXPECT_EQ(copy.type, soil.type);
	EXPECT_EQ(copy.properties, properties);
	EXPECT_EQ(copy.required_properties, required_properties);

	EXPECT_DOUBLE_EQ(copy.radius.value, radius.value);

	EXPECT_DOUBLE_EQ(copy.position.x, position.x);
	EXPECT_DOUBLE_EQ(copy.position.y, position.y);

	EXPECT_FLOAT_EQ(copy.damage_component.value().value, damage_value);
	EXPECT_FLOAT_EQ(copy.movement_cost_component.value().cost, movement_cost);
}

// Corpse: Full Serialization

TEST(CorpseSerializeDeserializeRoundtrip, FullCorpseSerialization) {
	const KShared::Core::Id::BaseIdType id = 248539453;

	const Karkinolution::Organisms::RawMeat raw_meat{std::numeric_limits<float>::max()};

	const KShared::Math::Vec::Vec2 position = {0.0, std::numeric_limits<double>::max()};

	const KShared::Math::Units::Size size{12.5, 24.0};

	Karkinolution::Organisms::Entities::Corpse::Corpse corpse{size, position, raw_meat, id};

	const auto                   serialized = CorpseSRI::serialize_corpse(corpse);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const auto copy = CorpseDSI::get_corpse(bytes, 0);

	EXPECT_EQ(copy.id, id);
	EXPECT_FLOAT_EQ(copy.raw_meat.value, raw_meat.value);

	EXPECT_DOUBLE_EQ(copy.position.x, position.x);
	EXPECT_DOUBLE_EQ(copy.position.y, position.y);

	EXPECT_DOUBLE_EQ(copy.size.width.value, size.width.value);
	EXPECT_DOUBLE_EQ(copy.size.height.value, size.height.value);
}

// Corpse: Request DSI

TEST(CorpseSerializeDeserializeRoundtrip, RequestDSI) {
	constexpr KShared::Core::Id::BaseIdType corpse_id = 248539453;

	const auto bytes = KShared::Binary::Serializer::serialize<KShared::Core::Id::BaseIdType>(corpse_id);
	const std::vector<std::byte> buffer(bytes.begin(), bytes.end());

	const auto interpreted_id = CorpseMessageDSI::interpret_like_see_corpse(buffer);

	EXPECT_EQ(interpreted_id, corpse_id);
}

// Territory: Size Roundtrip

TEST(TerritorySerializeDeserializeRoundtrip, FullFields) {
	const KShared::Math::Units::Size size{12.5, 24.0};

	const auto                   serialized = UnitsSRI::serialize_size(size);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const auto size_copy = UnitsDSI::deserialize_size(bytes, 0);

	EXPECT_DOUBLE_EQ(size_copy.width.value, size.width.value);
	EXPECT_DOUBLE_EQ(size_copy.height.value, size.height.value);
}

// Territory: Full Serialization

TEST(TerritorySerializeDeserializeRoundtrip, FullTerritorySerialization) {
	const KShared::Math::Units::Size size{12.5, 24.0};

	Karkinolution::Networking::Frames::World::Interpreters::Models::Territory::Territory territory{size};

	const auto                   serialized = TerritorySRI::serialize_territory(territory);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const auto copy = TerritoryDSI::deserialize_territory(bytes, 0);

	EXPECT_DOUBLE_EQ(copy.size.width.value, size.width.value);
	EXPECT_DOUBLE_EQ(copy.size.height.value, size.height.value);
}

// Creature: Full Serialization

TEST(CreatureSerializeDeserializeRoundtrip, FullCreatureSerialization) {
	const Karkinolution::Organisms::Entities::Creature::Gender gender =
		Karkinolution::Organisms::Entities::Creature::Gender::FEMALE;
	const Karkinolution::Organisms::Entities::Creature::CreatureSpecies specie =
		Karkinolution::Organisms::Entities::Creature::CreatureSpecies::FISH;

	const KShared::Math::Vec::Vec2 position{12.5, 24.0};

	const std::string name = "Karkinolution";

	const auto creature =
		Karkinolution::Organisms::Entities::Creature::Constructor::CreatureConstructor::from_blueprint(
			Karkinolution::Organisms::Entities::Creature::Constructor::CreatureBlueprint{.specie   = specie,
																						 .name     = name,
																						 .gender   = gender,
																						 .position = position});

	const auto                   serialized = CreatureSRI::serialize_creature(creature);
	const std::vector<std::byte> bytes(serialized.begin(), serialized.end());

	const auto copy = CreatureDSI::get_creature(bytes, 0);

	EXPECT_EQ(copy.gender, gender);
	EXPECT_EQ(copy.specie, specie);

	EXPECT_DOUBLE_EQ(copy.position.x, position.x);
	EXPECT_DOUBLE_EQ(copy.position.y, position.y);

	EXPECT_EQ(copy.name, name);
	EXPECT_DOUBLE_EQ(copy.morphology.size.width.value, creature.body.morphology.size.width.value);
	EXPECT_DOUBLE_EQ(copy.morphology.size.height.value, creature.body.morphology.size.height.value);
	EXPECT_DOUBLE_EQ(copy.morphology.mass.value, creature.body.morphology.mass.value);
	EXPECT_FLOAT_EQ(copy.vital.life.value, creature.body.vital.life.value());
	EXPECT_FLOAT_EQ(copy.vital.life.max, creature.body.vital.life.max());
	EXPECT_FLOAT_EQ(copy.vital.life.min, creature.body.vital.life.min());
	EXPECT_FLOAT_EQ(copy.vital.health.value, creature.body.vital.health.value());
	EXPECT_FLOAT_EQ(copy.vital.health.max, 1.0f);
	EXPECT_FLOAT_EQ(copy.vital.health.min, 0.0f);
	EXPECT_FLOAT_EQ(copy.metabolism.energy.value, creature.body.metabolism.energy.value());
	EXPECT_FLOAT_EQ(copy.metabolism.energy.max, creature.body.metabolism.energy.max());
	EXPECT_FLOAT_EQ(copy.metabolism.energy.min, creature.body.metabolism.energy.min());
	EXPECT_FLOAT_EQ(copy.metabolism.reserved_energy.value, creature.body.metabolism.reserved.value());
	EXPECT_FLOAT_EQ(copy.metabolism.reserved_energy.max, creature.body.metabolism.reserved.max());
	EXPECT_FLOAT_EQ(copy.metabolism.reserved_energy.min, 0.0f);
	EXPECT_FLOAT_EQ(copy.age, creature.body.age.value);
	EXPECT_EQ(copy.brain.intent.time, creature.brain.intent().time);
	EXPECT_EQ(copy.brain.intent.type, creature.brain.intent().type);
}

TEST(CreatureOntology, RandomGenderSelectionProducesBothGenders) {
	using Karkinolution::Organisms::Entities::Creature::choose_random_gender;
	using Karkinolution::Organisms::Entities::Creature::Gender;

	bool saw_female = false;
	bool saw_male   = false;

	for (int i = 0; i < 200; ++i) {
		auto g = choose_random_gender();
		if (g == Gender::FEMALE) {
			saw_female = true;
		} else if (g == Gender::MALE) {
			saw_male = true;
		}
		if (saw_female && saw_male) {
			break;
		}
	}

	EXPECT_TRUE(saw_female);
	EXPECT_TRUE(saw_male);
}
