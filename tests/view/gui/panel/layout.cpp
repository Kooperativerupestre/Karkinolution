#include <gtest/gtest.h>
#include <memory>
#include <opsiamethystos/gui/panel/layout/layout.hpp>

namespace Layout   = Opsiamethystos::Gui::Panel::Layout;
namespace Geometry = Opsiamethystos::Gui::Geometry;
using Opsiamethystos::Gui::Components::ComponentType;
using Opsiamethystos::Gui::Components::gen_id;

namespace {
	Layout::Node leaf(const Layout::Measure &measure = {}) {
		return Layout::Node{Layout::Leaf{gen_id(ComponentType::RECTANGLE), measure}};
	}

	std::shared_ptr<const Layout::Node> child_node(const Layout::Measure &measure = {}) {
		return std::make_shared<const Layout::Node>(leaf(measure));
	}

	Geometry::Rectangle slot(const Layout::Assignment &assignment) {
		return assignment.slot;
	}
} // namespace

TEST(GuiLayout, EmptyStackProducesNoAssignments) {
	const Layout::Node root{Layout::Stack{.direction = Layout::Direction::VERTICAL}};

	const auto result = Layout::layout({0, 0, 100, 100}, root);

	EXPECT_TRUE(result.assignments.empty());
	EXPECT_FALSE(result.overflowed);
}

TEST(GuiLayout, FillChildrenUseWeightedRemainingSpace) {
	const auto         first  = child_node();
	const auto         second = child_node();
	const auto         third  = child_node();
	const Layout::Node root{Layout::Stack{
		.direction = Layout::Direction::VERTICAL,
		.children  = {{Layout::Fill{1.0}, first}, {Layout::Fill{2.0}, second}, {Layout::Fill{1.0}, third}}}};

	const auto result = Layout::layout({0, 0, 20, 100}, root);

	ASSERT_EQ(result.assignments.size(), 3);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).height.value(), 25.0);
	EXPECT_DOUBLE_EQ(slot(result.assignments[1]).height.value(), 50.0);
	EXPECT_DOUBLE_EQ(slot(result.assignments[2]).height.value(), 25.0);
	EXPECT_DOUBLE_EQ(slot(result.assignments[2]).floor().value, 100.0F);
}

TEST(GuiLayout, ContentAndFixedChildrenLeaveRemainingSpaceForFill) {
	const auto         content = child_node([] {
        return Layout::Size{20, 15};
    });
	const auto         fixed   = child_node();
	const auto         fill    = child_node();
	const Layout::Node root{
		Layout::Stack{.direction = Layout::Direction::VERTICAL,
					  .spacing   = 5,
					  .children  = {{Layout::Content{}, content}, {Layout::Fixed{20}, fixed}, {Layout::Fill{}, fill}}}};

	const auto result = Layout::layout({0, 0, 100, 100}, root);

	ASSERT_EQ(result.assignments.size(), 3);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).height.value(), 15.0);
	EXPECT_DOUBLE_EQ(slot(result.assignments[1]).top().value, 20.0F);
	EXPECT_DOUBLE_EQ(slot(result.assignments[2]).height.value(), 55.0);
	EXPECT_DOUBLE_EQ(slot(result.assignments[2]).floor().value, 100.0F);
}

TEST(GuiLayout, OverflowClampsSlotsAndReportsOverflow) {
	const auto         first  = child_node();
	const auto         second = child_node();
	const Layout::Node root{Layout::Stack{.direction = Layout::Direction::VERTICAL,
										  .children  = {{Layout::Fixed{80}, first}, {Layout::Fixed{80}, second}}}};

	const auto result = Layout::layout({0, 0, 100, 100}, root);

	ASSERT_EQ(result.assignments.size(), 2);
	EXPECT_TRUE(result.overflowed);
	EXPECT_LE(result.assignments[0].slot.floor().value, 100.0F);
	EXPECT_LE(result.assignments[1].slot.floor().value, 100.0F);
	EXPECT_GE(result.assignments[0].slot.top().value, 0.0F);
	EXPECT_GE(result.assignments[1].slot.top().value, 0.0F);
}

TEST(GuiLayout, CenterAlignmentUsesPreferredCrossExtent) {
	const auto         child = child_node([] {
        return Layout::Size{20, 10};
    });
	const Layout::Node root{Layout::Stack{.direction   = Layout::Direction::VERTICAL,
										  .cross_align = Layout::CrossAlign::CENTER,
										  .children    = {{Layout::Fill{}, child}}}};

	const auto result = Layout::layout({0, 0, 100, 100}, root);

	ASSERT_EQ(result.assignments.size(), 1);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).width.value(), 20.0);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).position.x, 50.0F);
}

TEST(GuiLayout, PaddingAndSpacingReduceTheAvailableStack) {
	const auto         first  = child_node();
	const auto         second = child_node();
	const Layout::Node root{Layout::Stack{.direction = Layout::Direction::VERTICAL,
										  .padding   = {.left = 10, .top = 5, .right = 10, .floor = 15},
										  .spacing   = 5,
										  .children  = {{Layout::Fill{}, first}, {Layout::Fill{}, second}}}};

	const auto result = Layout::layout({0, 0, 100, 100}, root);

	ASSERT_EQ(result.assignments.size(), 2);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).left().value, 10.0F);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).top().value, 5.0F);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).height.value(), 37.5);
	EXPECT_DOUBLE_EQ(slot(result.assignments[1]).top().value, 47.5F);
	EXPECT_DOUBLE_EQ(slot(result.assignments[1]).floor().value, 85.0F);
}

TEST(GuiLayout, NestedHorizontalAndVerticalStacksKeepChildOrder) {
	const auto nested_leaf = child_node();
	const auto first =
		std::make_shared<const Layout::Node>(Layout::Node{Layout::Stack{.direction   = Layout::Direction::VERTICAL,
																		.cross_align = Layout::CrossAlign::STRETCH,
																		.children = {{Layout::Fill{}, nested_leaf}}}});
	const auto         second = child_node();
	const Layout::Node root{Layout::Stack{.direction   = Layout::Direction::HORIZONTAL,
										  .cross_align = Layout::CrossAlign::STRETCH,
										  .children    = {{Layout::Fixed{30}, first}, {Layout::Fixed{20}, second}}}};

	const auto result = Layout::layout({0, 0, 100, 50}, root);

	ASSERT_EQ(result.assignments.size(), 2);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).position.x, 15.0F);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).height.value(), 50.0);
	EXPECT_DOUBLE_EQ(slot(result.assignments[1]).position.x, 40.0F);
}

TEST(GuiLayout, FractionalFillDistributionEndsAtTheContainerEdge) {
	const auto         first  = child_node();
	const auto         second = child_node();
	const auto         third  = child_node();
	const Layout::Node root{
		Layout::Stack{.direction = Layout::Direction::VERTICAL,
					  .children  = {{Layout::Fill{1}, first}, {Layout::Fill{1}, second}, {Layout::Fill{1}, third}}}};

	const auto result = Layout::layout({0, 0, 10, 10}, root);

	ASSERT_EQ(result.assignments.size(), 3);
	EXPECT_FLOAT_EQ(slot(result.assignments[0]).top().value, 0.0F);
	EXPECT_FLOAT_EQ(slot(result.assignments[1]).top().value, 10.0F / 3.0F);
	EXPECT_FLOAT_EQ(slot(result.assignments[2]).top().value, 20.0F / 3.0F);
	EXPECT_FLOAT_EQ(slot(result.assignments[2]).floor().value, 10.0F);
}

TEST(GuiLayout, FillWithoutRemainingSpaceReportsOverflowAndKeepsAValidSlot) {
	const auto         fixed = child_node();
	const auto         fill  = child_node();
	const Layout::Node root{Layout::Stack{.direction = Layout::Direction::VERTICAL,
										  .children  = {{Layout::Fixed{10}, fixed}, {Layout::Fill{}, fill}}}};

	const auto result = Layout::layout({0, 0, 10, 10}, root);

	ASSERT_EQ(result.assignments.size(), 2);
	EXPECT_TRUE(result.overflowed);
	EXPECT_GE(slot(result.assignments[1]).height.value(), 1.0);
	EXPECT_LE(slot(result.assignments[1]).floor().value, 10.0F);
}

TEST(GuiLayout, ContentLargerThanTheContainerReportsOverflowAndClampsTheSlot) {
	const auto         content = child_node([] {
        return Layout::Size{20, 50};
    });
	const Layout::Node root{
		Layout::Stack{.direction = Layout::Direction::VERTICAL, .children = {{Layout::Content{}, content}}}};

	const auto result = Layout::layout({0, 0, 20, 10}, root);

	ASSERT_EQ(result.assignments.size(), 1);
	EXPECT_TRUE(result.overflowed);
	EXPECT_LE(slot(result.assignments[0]).floor().value, 10.0F);
	EXPECT_GE(slot(result.assignments[0]).top().value, 0.0F);
}

TEST(GuiLayout, PaddingLargerThanTheContainerStillProducesAContainedSlot) {
	const auto         child = child_node();
	const Layout::Node root{Layout::Stack{.direction   = Layout::Direction::VERTICAL,
										  .padding     = {.left = 20, .top = 20, .right = 20, .floor = 20},
										  .cross_align = Layout::CrossAlign::STRETCH,
										  .children    = {{Layout::Fill{}, child}}}};

	const auto result = Layout::layout({0, 0, 10, 10}, root);

	ASSERT_EQ(result.assignments.size(), 1);
	EXPECT_TRUE(result.overflowed);
	EXPECT_GE(slot(result.assignments[0]).width.value(), 1.0);
	EXPECT_GE(slot(result.assignments[0]).height.value(), 1.0);
	EXPECT_LE(slot(result.assignments[0]).right().value, 10.0F);
	EXPECT_LE(slot(result.assignments[0]).floor().value, 10.0F);
}

TEST(GuiLayout, SpacingDoesNotAffectAStackWithOneChild) {
	const auto         child = child_node();
	const Layout::Node root{
		Layout::Stack{.direction = Layout::Direction::VERTICAL, .spacing = 25, .children = {{Layout::Fill{}, child}}}};

	const auto result = Layout::layout({0, 0, 10, 10}, root);

	ASSERT_EQ(result.assignments.size(), 1);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).top().value, 0.0);
	EXPECT_DOUBLE_EQ(slot(result.assignments[0]).floor().value, 10.0);
}
