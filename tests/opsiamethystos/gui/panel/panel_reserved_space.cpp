#include <gtest/gtest.h>
#include <memory>
#include <opsiamethystos/graphics/panel/components/component.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/text.hpp>
#include <opsiamethystos/graphics/panel/layout/layout.hpp>
#include <opsiamethystos/graphics/panel/panel.hpp>
#include <shared/core/color.hpp>
#include <string_view>

namespace Panel      = Opsiamethystos::Gui::Panel;
namespace Layout     = Panel::Layout;
namespace Geometry   = Opsiamethystos::Math::Geometry;
namespace Components = Opsiamethystos::Gui::Components;
namespace LowLevel   = Components::LowLevel;

namespace {
	// Ten units of width per character, so "Disconnected" measures 120 and "Connected" 90.
	Components::Drivers::Font::TextMeasurement ten_per_character(const std::string_view text, const int32_t) {
		return {static_cast<float>(text.size()) * 10.0F, 10.0F};
	}
} // namespace

// A status label that switches text must keep a slot wide enough for its widest text.
TEST(GuiPanelReservedSpace, AMinimumKeepsTheSlotWideEnoughForTheWidestText) {
	Panel::Panel panel{Geometry::Rectangle{Geometry::Position{0, 0}, Geometry::Width{800}, Geometry::Height{450}}};
	const auto   label = std::make_shared<Components::Component>(Components::AllComponents{
        LowLevel::Text{Geometry::Rectangle{Geometry::Position{0, 0}, Geometry::Width{400}, Geometry::Height{50}},
                       "Disconnected",
                       KShared::Colors::Presets::White,
                       KShared::Math::LV::CT::GreaterThanZeroValue<int>{20},
                       ten_per_character}});
	auto        &text  = std::get<LowLevel::Text>(label->get_data());

	ASSERT_TRUE(panel.add(label));
	panel.set_layout(Layout::row({Layout::content(label, {.width = 120})}, {.cross_align = Layout::CrossAlign::START}));
	ASSERT_TRUE(panel.update().has_value());

	EXPECT_NO_THROW(text.set_text("Connected"));
	panel.mark_layout_dirty();
	ASSERT_TRUE(panel.update().has_value());

	EXPECT_NO_THROW(text.set_text("Disconnected"));
}
