#include <gtest/gtest.h>
#include <memory>
#include <opsiamethystos/graphics/panel/components/component.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/circle.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/rectangle.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/text.hpp>
#include <opsiamethystos/graphics/panel/layout/layout.hpp>
#include <shared/core/color.hpp>
#include <stdexcept>
#include <string_view>

namespace Layout     = Opsiamethystos::Gui::Panel::Layout;
namespace Geometry   = Opsiamethystos::Math::Geometry;
namespace Components = Opsiamethystos::Gui::Components;
namespace Drivers    = Components::Drivers;
namespace LowLevel   = Components::LowLevel;
using KShared::Colors::Presets::White;

namespace {
	// Two units of width per character, so the measured size follows the text.
	Drivers::Font::TextMeasurement width_per_character(const std::string_view text, const int32_t) {
		return {static_cast<float>(text.size()) * 2.0F, 10.0F};
	}

	std::shared_ptr<Components::Component> text_component() {
		return std::make_shared<Components::Component>(Components::AllComponents{
			LowLevel::Text{Geometry::Rectangle{Geometry::Position{0, 0}, Geometry::Width{80}, Geometry::Height{30}},
						   "abcd",
						   White,
						   KShared::Math::LV::CT::GreaterThanZeroValue<int>{10},
						   width_per_character}});
	}
} // namespace

TEST(GuiLayoutParticipant, ComponentsExposeTheirRegisteredIdAsTheLayoutId) {
	const auto rectangle = std::make_shared<Components::Component>(Components::AllComponents{
		LowLevel::Rectangle{Geometry::Rectangle{Geometry::Position{0, 0}, Geometry::Width{30}, Geometry::Height{12}},
							White}});
	const auto circle    = std::make_shared<Components::Component>(Components::AllComponents{
        LowLevel::Circle{Geometry::Circle{Geometry::Position{0, 0}, Geometry::Radius{7}}, White}});
	const auto text      = text_component();

	for (const std::shared_ptr<const Layout::Participant> participant : {rectangle, circle, text}) {
		EXPECT_EQ(participant->layout_id(),
				  std::dynamic_pointer_cast<const Components::Component>(participant)->get_id());
	}
}

TEST(GuiLayoutParticipant, PreferredSizeIsTheCurrentBoundOfEachShape) {
	const auto rectangle = std::make_shared<Components::Component>(Components::AllComponents{
		LowLevel::Rectangle{Geometry::Rectangle{Geometry::Position{0, 0}, Geometry::Width{30}, Geometry::Height{12}},
							White}});
	const auto circle    = std::make_shared<Components::Component>(Components::AllComponents{
        LowLevel::Circle{Geometry::Circle{Geometry::Position{0, 0}, Geometry::Radius{7}}, White}});
	const auto text      = text_component();

	EXPECT_EQ(rectangle->preferred_size().width, 30.0);
	EXPECT_EQ(rectangle->preferred_size().height, 12.0);
	EXPECT_EQ(circle->preferred_size().width, 14.0);
	EXPECT_EQ(circle->preferred_size().height, 14.0);
	// Text reports its measured bound, not the larger maximum bound it was indexed with.
	EXPECT_EQ(text->preferred_size().width, 8.0);
	EXPECT_EQ(text->preferred_size().height, 10.0);
}

TEST(GuiLayoutParticipant, LeafUsesTheParticipantIdAndReadsItsPreferredSizeOnEveryCall) {
	const auto  text = text_component();
	const auto  node = Layout::leaf(text);
	const auto &leaf = std::get<Layout::Leaf>(node.value);

	EXPECT_EQ(leaf.id, text->get_id());
	EXPECT_EQ(leaf.measure().width, 8.0);

	std::get<LowLevel::Text>(text->get_data()).set_text("abcdef");

	EXPECT_EQ(leaf.measure().width, 12.0);
}

TEST(GuiLayoutParticipant, LeafKeepsTheParticipantAliveForAsLongAsTheLeafExists) {
	auto                                             text = text_component();
	const std::weak_ptr<const Components::Component> weak = text;
	auto                                             node = std::make_unique<Layout::Node>(Layout::leaf(text));

	text.reset();
	EXPECT_FALSE(weak.expired());
	EXPECT_EQ(std::get<Layout::Leaf>(node->value).measure().width, 8.0);

	node.reset();
	EXPECT_TRUE(weak.expired());
}

TEST(GuiLayoutParticipant, LeafRejectsANullParticipant) {
	EXPECT_THROW(static_cast<void>(Layout::leaf(nullptr)), std::invalid_argument);
}
