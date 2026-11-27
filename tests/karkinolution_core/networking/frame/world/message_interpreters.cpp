#include "karkinolution/networking/frame/world/message_interpreters/creature.hpp"
#include "shared/binary/errors.hpp"
#include "shared/core/error.hpp"

#include <gtest/gtest.h>

TEST(WorldMessageInterpreters, CreateCreatureRequestRoundTripsPresetFields) {
	using Gender  = Karkinolution::Organisms::Entities::Creature::Gender;
	using Species = Karkinolution::Organisms::Entities::Creature::CreatureSpecies;
	using Preset  = Karkinolution::Networking::Frames::World::MessageInterpreters::Models::Creature::CreaturePreset;

	const Preset                 preset{.gender   = Gender::FEMALE,
										.specie   = Species::FISH,
										.position = KShared::Math::Vec::Vec2{12.5, -4.25},
										.name     = "Mira"};
	const auto                   payload = CreatureMessageSRI::make_create_creature(preset);
	const std::vector<std::byte> bytes(payload.begin(), payload.end());

	const auto decoded = CreatureMessageDSI::interpret_like_create_creature(bytes);

	EXPECT_EQ(decoded.gender, preset.gender);
	EXPECT_EQ(decoded.specie, preset.specie);
	ASSERT_TRUE(decoded.position.has_value());
	EXPECT_DOUBLE_EQ(decoded.position->x, preset.position->x);
	EXPECT_DOUBLE_EQ(decoded.position->y, preset.position->y);
	EXPECT_EQ(decoded.name, preset.name);
}

TEST(WorldMessageInterpreters, CreateCreatureRequestPreservesAbsentFields) {
	const auto                   payload = CreatureMessageSRI::make_create_creature({});
	const std::vector<std::byte> bytes(payload.begin(), payload.end());

	const auto decoded = CreatureMessageDSI::interpret_like_create_creature(bytes);

	EXPECT_FALSE(decoded.gender.has_value());
	EXPECT_FALSE(decoded.specie.has_value());
	EXPECT_FALSE(decoded.position.has_value());
	EXPECT_FALSE(decoded.name.has_value());
}

TEST(WorldMessageInterpreters, CreateCreatureRequestRejectsNamesLargerThanTheWireSlot) {
	using Preset = Karkinolution::Networking::Frames::World::MessageInterpreters::Models::Creature::CreaturePreset;
	const auto oversized_name = Preset{.name = std::string(CreatureSRI::Layout::MAX_NAME_SIZE + 1, 'x')};
	EXPECT_THROW(CreatureMessageSRI::make_create_creature(oversized_name), KShared::Errors::SimulationError);

	auto payload                                  = CreatureMessageSRI::make_create_creature(Preset{.name = "x"});
	payload[Karkinolution::Networking::Frames::World::MessageInterpreters::SRI::Creature::Request::Layout::
				CreateCreature::NameSize::OFFSET] = std::byte{65};
	const std::vector<std::byte> bytes(payload.begin(), payload.end());
	EXPECT_THROW(CreatureMessageDSI::interpret_like_create_creature(bytes), KShared::Binary::ByteError);
}
