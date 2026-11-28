#include <gtest/gtest.h>
#include <limits>
#include <opsiamethystos/graphics/panel/layout/layout.hpp>
#include <stdexcept>

namespace Layout   = Opsiamethystos::Gui::Panel::Layout;
namespace Geometry = Opsiamethystos::Math::Geometry;
using Opsiamethystos::Gui::Components::ComponentType;
using Opsiamethystos::Gui::Components::gen_id;

namespace {
	constexpr double not_a_number = std::numeric_limits<double>::quiet_NaN();

	// A leaf that always reports width x height.
	Layout::Node measured(const double width, const double height) {
		return Layout::Node{Layout::Leaf{gen_id(ComponentType::RECTANGLE), [width, height] {
											 return Layout::Size{width, height};
										 }}};
	}

	Geometry::Rectangle edges(const float left, const float top, const float right, const float floor) {
		return Geometry::Rectangle::from_edges(Geometry::Left{left},
											   Geometry::Top{top},
											   Geometry::Right{right},
											   Geometry::Floor{floor});
	}

	Layout::Edges panel_box() {
		return {0, 0, 100, 50};
	}
} // namespace

TEST(GuiLayoutMinimum, ContentWithoutAMinimumStillTakesItsPreferredSize) {
	const auto root = Layout::row({Layout::content(measured(10, 10))}, {.cross_align = Layout::CrossAlign::START});

	const auto result = Layout::layout(panel_box(), root);

	ASSERT_EQ(result.assignments.size(), 1U);
	EXPECT_EQ(result.assignments[0].slot, edges(0, 0, 10, 10));
}

TEST(GuiLayoutMinimum, ContentNeverGetsLessThanItsMinimumOnTheMainAxis) {
	const auto root = Layout::row({Layout::content(measured(10, 10), {.width = 40}), Layout::fill(measured(1, 1))},
								  {.cross_align = Layout::CrossAlign::START});

	const auto result = Layout::layout(panel_box(), root);

	ASSERT_EQ(result.assignments.size(), 2U);
	EXPECT_EQ(result.assignments[0].slot, edges(0, 0, 40, 10));
	EXPECT_EQ(result.assignments[1].slot.left().value, 40.0F);
}

TEST(GuiLayoutMinimum, ContentGrowsPastItsMinimumWhenThePreferredSizeIsLarger) {
	const auto root =
		Layout::row({Layout::content(measured(60, 10), {.width = 40})}, {.cross_align = Layout::CrossAlign::START});

	const auto result = Layout::layout(panel_box(), root);

	EXPECT_EQ(result.assignments[0].slot, edges(0, 0, 60, 10));
}

TEST(GuiLayoutMinimum, TheMinimumAlsoAppliesToTheCrossAxis) {
	const auto root =
		Layout::row({Layout::content(measured(10, 10), {.height = 30})}, {.cross_align = Layout::CrossAlign::START});

	const auto result = Layout::layout(panel_box(), root);

	EXPECT_EQ(result.assignments[0].slot, edges(0, 0, 10, 30));
}

TEST(GuiLayoutMinimum, AMinimumLargerThanTheAvailableSpaceIsReportedAsOverflow) {
	const auto root =
		Layout::row({Layout::content(measured(10, 10), {.width = 150})}, {.cross_align = Layout::CrossAlign::START});

	const auto result = Layout::layout(panel_box(), root);

	EXPECT_TRUE(result.overflowed);
}

// The purpose of a minimum: a changing preferred size must not move the slot.
TEST(GuiLayoutMinimum, TheSlotStaysStableWhileThePreferredSizeChangesBelowTheMinimum) {
	double     current_width = 90.0;
	const auto changing      = Layout::Node{Layout::Leaf{gen_id(ComponentType::RECTANGLE), [&current_width] {
                                                        return Layout::Size{current_width, 10};
                                                    }}};
	const auto root =
		Layout::row({Layout::content(changing, {.width = 90})}, {.cross_align = Layout::CrossAlign::START});

	for (const double width : {90.0, 40.0, 90.0, 10.0}) {
		current_width     = width;
		const auto result = Layout::layout(panel_box(), root);
		EXPECT_EQ(result.assignments[0].slot, edges(0, 0, 90, 10)) << "preferred width " << width;
	}
}

TEST(GuiLayoutMinimum, InvalidMinimumsAreRejected) {
	EXPECT_THROW(static_cast<void>(Layout::content(measured(1, 1), {.width = -1})), std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::content(measured(1, 1), {.height = not_a_number})), std::invalid_argument);

	const Layout::Node hand_built{Layout::Stack{
		.children = {{Layout::Content{{.width = -5}}, std::make_shared<const Layout::Node>(measured(1, 1))}}}};
	EXPECT_THROW(Layout::validate(hand_built), std::invalid_argument);
}
