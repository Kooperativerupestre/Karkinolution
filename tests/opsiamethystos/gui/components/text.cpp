#include <gtest/gtest.h>
#include <opsiamethystos/graphics/panel/components/low_level/text.hpp>
#include <shared/core/color.hpp>
#include <string_view>

namespace Geometry   = Opsiamethystos::Math::Geometry;
namespace Components = Opsiamethystos::Gui::Components;
namespace LowLevel   = Opsiamethystos::Graphics::Panel::Components::LowLevel;
using KShared::Colors::Presets::White;

namespace {
	// Ten units of width per character: "abcd" measures 40 and "ab" measures 20.
	Components::Drivers::Font::TextMeasurement ten_per_character(const std::string_view text, const int32_t) {
		return {static_cast<float>(text.size()) * 10.0F, 10.0F};
	}

	Geometry::Rectangle slot(const float x, const float y, const double width, const double height) {
		return {Geometry::Position{x, y}, Geometry::Width{width}, Geometry::Height{height}};
	}

	LowLevel::Text text_in(const Geometry::Rectangle &where, std::string text) {
		return LowLevel::Text{where,
							  std::move(text),
							  White,
							  KShared::Math::LV::CT::GreaterThanZeroValue<int>{20},
							  ten_per_character};
	}
} // namespace

// What a text can do must not depend on the slot of an earlier layout pass.
TEST(GuiText, AcceptsATextLargerThanItsSlotEverywhere) {
	LowLevel::Text text{text_in(slot(0, 0, 20, 10), "abcd")};

	EXPECT_EQ(text.get_current_bounds().width.value(), 40.0);
	EXPECT_NO_THROW(text.set_text("abcdef"));
	EXPECT_EQ(text.get_current_bounds().width.value(), 60.0);
	EXPECT_TRUE(text.set_slot(slot(0, 0, 10, 10)));
	EXPECT_EQ(text.get_current_bounds().width.value(), 60.0);
}

TEST(GuiText, OverflowIsCenteredOnTheSlotAndTheSlotStaysTheIndexedBound) {
	LowLevel::Text text{text_in(slot(0, 0, 100, 100), "abcd")};
	const auto     narrow = slot(30, 40, 20, 10);

	ASSERT_TRUE(text.set_slot(narrow));

	EXPECT_EQ(text.get_bounds(), narrow);
	EXPECT_FLOAT_EQ(text.get_current_bounds().position.x, 30.0F);
	EXPECT_FLOAT_EQ(text.get_current_bounds().position.y, 40.0F);
	EXPECT_EQ(text.get_current_bounds().width.value(), 40.0);
}

TEST(GuiText, ChangingTheTextRemeasuresWithoutMovingTheSlot) {
	const auto     where = slot(10, 20, 100, 30);
	LowLevel::Text text{text_in(where, "abcd")};

	text.set_text("ab");

	EXPECT_EQ(text.get_text(), "ab");
	EXPECT_EQ(text.get_bounds(), where);
	EXPECT_EQ(text.get_current_bounds().width.value(), 20.0);
	EXPECT_FLOAT_EQ(text.get_current_bounds().position.x, 10.0F);
}

TEST(GuiText, MovingToALargerSlotKeepsTheTextMeasuredAndCentered) {
	LowLevel::Text text{text_in(slot(0, 0, 20, 10), "abcd")};
	const auto     roomy = slot(-20, 5, 200, 50);

	ASSERT_TRUE(text.set_slot(roomy));

	EXPECT_EQ(text.get_bounds(), roomy);
	EXPECT_EQ(text.get_current_bounds().width.value(), 40.0);
	EXPECT_FLOAT_EQ(text.get_current_bounds().position.x, -20.0F);
}
