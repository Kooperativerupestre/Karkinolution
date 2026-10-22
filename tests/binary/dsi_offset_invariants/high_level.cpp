#include <gtest/gtest.h>
#include <karkinolution/binary/interpreters/corpse.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/binary/interpreters/geometry.hpp>
#include <karkinolution/binary/interpreters/properties.hpp>
#include <karkinolution/binary/interpreters/soil.hpp>
#include <karkinolution/binary/interpreters/vec.hpp>
#include <karkinolution/binary/serializer.hpp>
#include <karkinolution/core/id.hpp>
#include <karkinolution/organism/entities/creature/constructor.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/terrain/soil.hpp>

TEST(DSIOffsetInvariantsHighLevel, CreatureAuxiliaryGettersDoNotAutoAddOffset) {
	{
		std::vector<std::byte> payload = {std::byte{0xFF}, std::byte{0xFF}, std::byte{0x01}};
		const auto             gender  = CreatureDSI::get_gender(payload, 2);
		EXPECT_EQ(gender, Gender::FEMALE);
	}

	{
		std::vector<std::byte> payload = {std::byte{0x00},
										  std::byte{0x00},
										  static_cast<std::byte>(CreatureSpecies::FISH)};
		const auto             specie  = CreatureDSI::get_specie(payload, 2);
		EXPECT_EQ(specie, CreatureSpecies::FISH);
	}

	{
		const std::string      test_name       = "Crab";
		const auto             serialized_name = CreatureSRI::serialize_name(test_name);
		std::vector<std::byte> payload         = {std::byte{0xAA}, std::byte{0xBB}};
		const std::size_t      offset          = payload.size();
		BinaryUtils::append_bytes(payload, serialized_name);

		EXPECT_EQ(CreatureDSI::get_name_size(payload, offset),
				  static_cast<uint8_t>(test_name.size()));
		EXPECT_EQ(CreatureDSI::get_name(payload, offset), test_name);
	}

	{
		const BaseIdType       expected_id = 987654321;
		const auto             id_bytes    = Serializer::serialize<BaseIdType>(expected_id);
		std::vector<std::byte> payload     = {std::byte{0x12}, std::byte{0x34}, std::byte{0x56}};
		const std::size_t      offset      = payload.size();
		BinaryUtils::append_bytes(payload, id_bytes);

		EXPECT_EQ(CreatureDSI::get_id(payload, offset), expected_id);
	}
}

TEST(DSIOffsetInvariantsHighLevel, SoilAuxiliaryGettersDoNotAutoAddOffset) {
	{
		std::vector<std::byte> payload = {std::byte{0xFF}, static_cast<std::byte>(SoilTypes::SAND)};
		EXPECT_EQ(SoilDSI::get_type(payload, 1), SoilTypes::SAND);
	}

	{
		const BaseIdType       expected_id = 1122334455;
		const auto             id_bytes    = Serializer::serialize<BaseIdType>(expected_id);
		std::vector<std::byte> payload     = {std::byte{0xDE}, std::byte{0xAD}};
		const std::size_t      offset      = payload.size();
		BinaryUtils::append_bytes(payload, id_bytes);

		EXPECT_EQ(SoilDSI::get_id(payload, offset), expected_id);
	}

	{
		const Radius           original_radius{42.5};
		const auto             radius_bytes = GeometrySRI::serialize_radius(original_radius);
		std::vector<std::byte> payload      = {std::byte{0x01}, std::byte{0x02}, std::byte{0x03}};
		const std::size_t      offset       = payload.size();
		BinaryUtils::append_bytes(payload, radius_bytes);

		EXPECT_DOUBLE_EQ(SoilDSI::get_radius(payload, offset).value, original_radius.value);
	}

	{
		const Vec3             original_pos{1.0, 2.0, 3.0};
		const auto             pos_bytes = VecSRI::serialize_vec(original_pos);
		std::vector<std::byte> payload   = {std::byte{0xAA}};
		const std::size_t      offset    = payload.size();
		BinaryUtils::append_bytes(payload, pos_bytes);

		const auto read_pos = SoilDSI::get_position(payload, offset);
		EXPECT_DOUBLE_EQ(read_pos.x, original_pos.x);
		EXPECT_DOUBLE_EQ(read_pos.y, original_pos.y);
		EXPECT_DOUBLE_EQ(read_pos.z, original_pos.z);
	}

	{
		SoilPiece soil{.type     = SoilTypes::DIRT,
					   .radius   = Radius{1.0},
					   .position = {0, 0, 0},
					   .id       = 1};
		soil.components.add(SoilPieceComponents::Damage{15.5f});
		const auto damage_bytes = SoilSRI::serialize_damage(soil);

		std::vector<std::byte> payload = {std::byte{0xEE}, std::byte{0xFF}};
		const std::size_t      offset  = payload.size();
		BinaryUtils::append_bytes(payload, damage_bytes);

		const auto damage_opt = SoilDSI::get_damage(payload, offset);
		ASSERT_TRUE(damage_opt.has_value());
		EXPECT_FLOAT_EQ(damage_opt->value, 15.5f);
	}

	{
		SoilPiece soil{.type     = SoilTypes::DIRT,
					   .radius   = Radius{1.0},
					   .position = {0, 0, 0},
					   .id       = 1};
		soil.components.add(SoilPieceComponents::MovementCost{2.5f});
		const auto mc_bytes = SoilSRI::serialize_movement_cost(soil);

		std::vector<std::byte> payload = {std::byte{0x11}, std::byte{0x22}};
		const std::size_t      offset  = payload.size();
		BinaryUtils::append_bytes(payload, mc_bytes);

		const auto mc_opt = SoilDSI::get_movement_cost(payload, offset);
		ASSERT_TRUE(mc_opt.has_value());
		EXPECT_FLOAT_EQ(mc_opt->cost, 2.5f);
	}

	{
		const std::vector<SoilProperties> original_props = {SoilProperties::DANGEROUS};
		const auto prop_bytes = SoilSRI::serialize_properties(original_props);

		std::vector<std::byte> payload = {std::byte{0x55}};
		const std::size_t      offset  = payload.size();
		BinaryUtils::append_bytes(payload, prop_bytes);

		EXPECT_EQ(SoilDSI::get_soil_properties(payload, offset), original_props);
	}

	{
		const std::vector<GenericProperty> original_caps = {
			GenericProperty(Properties::Capabilities::Move::SWIMM)};
		const auto caps_bytes = SoilSRI::serialize_required_capabilities(original_caps);

		std::vector<std::byte> payload = {std::byte{0x77}, std::byte{0x88}};
		const std::size_t      offset  = payload.size();
		BinaryUtils::append_bytes(payload, caps_bytes);

		EXPECT_EQ(SoilDSI::get_required_properties(payload, offset), original_caps);
	}
}

TEST(DSIOffsetInvariantsHighLevel, CorpseAuxiliaryGettersDoNotAutoAddOffset) {
	{
		const BaseIdType       expected_id = 99887766;
		const auto             id_bytes    = CorpseSRI::serialize_id(expected_id);
		std::vector<std::byte> payload     = {std::byte{0x01}, std::byte{0x02}};
		const std::size_t      offset      = payload.size();
		BinaryUtils::append_bytes(payload, id_bytes);

		EXPECT_EQ(CorpseDSI::get_id(payload, offset), expected_id);
	}

	{
		const OrganismStats::Body::RawMeat original_meat{45.25f};
		const auto             meat_bytes = CorpseSRI::serialize_raw_meat(original_meat);
		std::vector<std::byte> payload    = {std::byte{0xAA}, std::byte{0xBB}, std::byte{0xCC}};
		const std::size_t      offset     = payload.size();
		BinaryUtils::append_bytes(payload, meat_bytes);

		EXPECT_FLOAT_EQ(CorpseDSI::get_raw_meat(payload, offset).value, original_meat.value);
	}

	{
		const Vec3             original_pos{10.0, 20.0, 30.0};
		const auto             pos_bytes = CorpseSRI::serialize_position(original_pos);
		std::vector<std::byte> payload   = {std::byte{0x99}};
		const std::size_t      offset    = payload.size();
		BinaryUtils::append_bytes(payload, pos_bytes);

		const auto read_pos = CorpseDSI::get_position(payload, offset);
		EXPECT_DOUBLE_EQ(read_pos.x, original_pos.x);
		EXPECT_DOUBLE_EQ(read_pos.y, original_pos.y);
		EXPECT_DOUBLE_EQ(read_pos.z, original_pos.z);
	}

	{
		const Size             original_size{4.0, 5.0, 6.0};
		const auto             size_bytes = CorpseSRI::serialize_size(original_size);
		std::vector<std::byte> payload    = {std::byte{0x12}, std::byte{0x34}};
		const std::size_t      offset     = payload.size();
		BinaryUtils::append_bytes(payload, size_bytes);

		const auto read_size = CorpseDSI::get_size(payload, offset);
		EXPECT_DOUBLE_EQ(read_size.lateral.value, original_size.lateral.value);
		EXPECT_DOUBLE_EQ(read_size.height.value, original_size.height.value);
		EXPECT_DOUBLE_EQ(read_size.depth.value, original_size.depth.value);
	}
}
