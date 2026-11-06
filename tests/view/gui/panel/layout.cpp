#include <gtest/gtest.h>
#include <karkinolution/view/gui/panel/layout/layout.hpp>
#include <memory>

namespace Layout   = KarkinolutionView::Gui::Panel::Layout;
namespace Geometry = KarkinolutionView::Gui::Geometry;
using KarkinolutionView::Gui::Components::ComponentType;
using KarkinolutionView::Gui::Components::gen_id;

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
