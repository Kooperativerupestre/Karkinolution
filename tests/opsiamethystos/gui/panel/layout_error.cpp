#include <array>
#include <gtest/gtest.h>
#include <opsiamethystos/graphics/panel/layout_error.hpp>
#include <set>
#include <string_view>

namespace Panel = Opsiamethystos::Gui::Panel;

// Diagnostics are only useful if every failure reason maps to its own non-empty text.
TEST(GuiLayoutError, EveryCodeHasADistinctNonEmptyDescription) {
	constexpr std::array codes{Panel::LayoutErrorCode::UNKNOWN_COMPONENT,
							   Panel::LayoutErrorCode::SLOT_OUTSIDE_PANEL,
							   Panel::LayoutErrorCode::COMPONENT_REJECTED_SLOT,
							   Panel::LayoutErrorCode::INDEX_REJECTED_SLOT};

	std::set<std::string_view> descriptions;
	for (const auto code : codes) {
		const auto text = Panel::to_string(code);
		EXPECT_FALSE(text.empty());
		EXPECT_TRUE(descriptions.insert(text).second);
	}
	EXPECT_EQ(descriptions.size(), codes.size());
}
