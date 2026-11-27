#include <cmath>
#include <gtest/gtest.h>
#include <limits>
#include <memory>
#include <opsiamethystos/graphics/panel/components/component.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/rectangle.hpp>
#include <opsiamethystos/graphics/panel/layout/layout.hpp>
#include <shared/core/color.hpp>
#include <stdexcept>

namespace Layout     = Opsiamethystos::Gui::Panel::Layout;
namespace Geometry   = Opsiamethystos::Math::Geometry;
namespace Components = Opsiamethystos::Gui::Components;
namespace LowLevel   = Components::LowLevel;
using KShared::Colors::Presets::White;

namespace {
	constexpr double not_a_number = std::numeric_limits<double>::quiet_NaN();

	// A rectangle component whose preferred size is exactly width x height.
	std::shared_ptr<Components::Component> sized(const double width, const double height) {
		return std::make_shared<Components::Component>(Components::AllComponents{LowLevel::Rectangle{
			Geometry::Rectangle{Geometry::Position{0, 0}, Geometry::Width{width}, Geometry::Height{height}},
			White}});
	}

	Geometry::Rectangle edges(const float left, const float top, const float right, const float floor) {
		return Geometry::Rectangle::from_edges(Geometry::Left{left},
											   Geometry::Top{top},
											   Geometry::Right{right},
											   Geometry::Floor{floor});
	}
} // namespace

TEST(GuiLayoutBuilders, RowAndColumnSetTheirDirectionAndCarryTheOptions) {
	const auto                 child = sized(10, 10);
	const Layout::StackOptions options{.padding     = {.left = 1, .top = 2, .right = 3, .floor = 4},
									   .spacing     = 5,
									   .cross_align = Layout::CrossAlign::CENTER};

	const auto row    = Layout::row({Layout::fixed(10, child)}, options);
	const auto column = Layout::column({Layout::fixed(10, sized(10, 10))}, options);

	const auto &row_stack    = std::get<Layout::Stack>(row.value);
	const auto &column_stack = std::get<Layout::Stack>(column.value);
	EXPECT_EQ(row_stack.direction, Layout::Direction::HORIZONTAL);
	EXPECT_EQ(column_stack.direction, Layout::Direction::VERTICAL);
	for (const auto* stack : {&row_stack, &column_stack}) {
		EXPECT_EQ(stack->spacing, 5.0);
		EXPECT_EQ(stack->padding.left, 1.0);
		EXPECT_EQ(stack->padding.top, 2.0);
		EXPECT_EQ(stack->padding.right, 3.0);
		EXPECT_EQ(stack->padding.floor, 4.0);
		EXPECT_EQ(stack->cross_align, Layout::CrossAlign::CENTER);
		EXPECT_EQ(stack->children.size(), 1U);
	}
}

TEST(GuiLayoutBuilders, StacksStretchAcrossTheirAxisByDefault) {
	const auto row = Layout::row({});

	EXPECT_EQ(std::get<Layout::Stack>(row.value).cross_align, Layout::CrossAlign::STRETCH);
}

TEST(GuiLayoutBuilders, PolicyBuildersProduceTheRequestedPolicy) {
	const auto fixed        = Layout::fixed(40, sized(1, 1));
	const auto content      = Layout::content(sized(1, 1));
	const auto default_fill = Layout::fill(sized(1, 1));
	const auto weighted     = Layout::fill(sized(1, 1), 2.5);

	EXPECT_EQ(std::get<Layout::Fixed>(fixed.policy).extent, 40.0);
	EXPECT_TRUE(std::holds_alternative<Layout::Content>(content.policy));
	EXPECT_EQ(std::get<Layout::Fill>(default_fill.policy).weight, 1.0);
	EXPECT_EQ(std::get<Layout::Fill>(weighted.policy).weight, 2.5);
	EXPECT_TRUE(fixed.node != nullptr && content.node != nullptr && default_fill.node != nullptr);
}

TEST(GuiLayoutBuilders, BuildersRejectInvalidArgumentsAtTheCallSite) {
	const auto child = sized(1, 1);

	EXPECT_THROW(static_cast<void>(Layout::fixed(0, child)), std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::fixed(not_a_number, child)), std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::fill(child, -1.0)), std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::fill(child, not_a_number)), std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::fixed(10, std::shared_ptr<const Layout::Participant>{})),
				 std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::content(std::shared_ptr<const Layout::Participant>{})),
				 std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::fill(std::shared_ptr<const Layout::Participant>{})), std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::row({Layout::fixed(10, sized(1, 1))}, {.spacing = -1})),
				 std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::column({Layout::fixed(10, sized(1, 1))}, {.padding = {.left = -1}})),
				 std::invalid_argument);
}

TEST(GuiLayoutBuilders, RowAndColumnRejectTheSameComponentTwice) {
	const auto child = sized(1, 1);

	EXPECT_THROW(static_cast<void>(Layout::row({Layout::fixed(10, child), Layout::fill(child)})),
				 std::invalid_argument);
	EXPECT_THROW(static_cast<void>(Layout::column({Layout::fixed(10, child), Layout::content(child)})),
				 std::invalid_argument);
}

// The builders are only a way to write a tree; the layout they describe must be identical.
TEST(GuiLayoutBuilders, BuiltRowLaysOutExactlyLikeTheHandBuiltStack) {
	const auto first  = sized(15, 10);
	const auto second = sized(15, 10);
	const auto third  = sized(35, 20);

	const auto built = Layout::row({Layout::fixed(20, first), Layout::fill(second), Layout::content(third)},
								   {.padding     = {.left = 4, .top = 6, .right = 4, .floor = 6},
									.spacing     = 5,
									.cross_align = Layout::CrossAlign::START});

	const auto leaf_of = [](const std::shared_ptr<Components::Component> &component) {
		return std::make_shared<const Layout::Node>(Layout::Node{Layout::Leaf{component->get_id(), [component] {
																				  return component->preferred_size();
																			  }}});
	};
	const Layout::Node hand_built{Layout::Stack{.direction   = Layout::Direction::HORIZONTAL,
												.padding     = {.left = 4, .top = 6, .right = 4, .floor = 6},
												.spacing     = 5,
												.cross_align = Layout::CrossAlign::START,
												.children    = {{Layout::Fixed{20}, leaf_of(first)},
																{Layout::Fill{1.0}, leaf_of(second)},
																{Layout::Content{}, leaf_of(third)}}}};

	const auto expected = Layout::layout({0, 0, 100, 50}, hand_built);
	const auto actual   = Layout::layout({0, 0, 100, 50}, built);

	ASSERT_EQ(actual.assignments.size(), expected.assignments.size());
	for (std::size_t index = 0; index < actual.assignments.size(); ++index) {
		EXPECT_EQ(actual.assignments[index].id, expected.assignments[index].id);
		EXPECT_EQ(actual.assignments[index].slot, expected.assignments[index].slot);
	}
	EXPECT_EQ(actual.overflowed, expected.overflowed);
}

TEST(GuiLayoutBuilders, NestedRowsInsideAColumnLayOutEveryLeafInsideItsRow) {
	const auto first  = sized(1, 1);
	const auto second = sized(1, 1);
	const auto third  = sized(1, 1);

	const auto root = Layout::column(
		{Layout::fixed(30, Layout::row({Layout::fixed(20, first), Layout::fill(second)})), Layout::fill(third)});

	const auto result = Layout::layout({0, 0, 100, 100}, root);

	ASSERT_EQ(result.assignments.size(), 3U);
	EXPECT_EQ(result.assignments[0].slot, edges(0, 0, 20, 30));
	EXPECT_EQ(result.assignments[1].slot, edges(20, 0, 100, 30));
	EXPECT_EQ(result.assignments[2].slot, edges(0, 30, 100, 100));
	EXPECT_FALSE(result.overflowed);
}
