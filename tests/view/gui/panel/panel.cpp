#include <gtest/gtest.h>
#include <karkinolution/core/color.hpp>
#include <karkinolution/view/gui/components/component.hpp>
#include <karkinolution/view/gui/components/low_level/circle.hpp>
#include <karkinolution/view/gui/components/low_level/rectangle.hpp>
#include <karkinolution/view/gui/components/low_level/text.hpp>
#include <karkinolution/view/gui/panel/panel.hpp>
#include <memory>

namespace {
	namespace Geometry   = KarkinolutionView::Gui::Geometry;
	namespace Components = KarkinolutionView::Gui::Components;
	namespace LowLevel   = Components::LowLevel;
	namespace Panel      = KarkinolutionView::Gui::Panel;
	using Karkinolution::Colors::Presets::White;

	Geometry::Rectangle rectangle(float x, float y, double width = 10.0, double height = 10.0) {
		return {Geometry::Position{x, y}, Geometry::Width{width}, Geometry::Height{height}};
	}

	std::shared_ptr<Components::Component> rectangle_component(const Geometry::Rectangle &bounds) {
		return std::make_shared<Components::Component>(Components::AllComponents{LowLevel::Rectangle{bounds, White}});
	}

	std::shared_ptr<Components::Component> circle_component(const Geometry::Circle &bounds) {
		return std::make_shared<Components::Component>(Components::AllComponents{LowLevel::Circle{bounds, White}});
	}
} // namespace

TEST(GuiPanel, SetBoundsMovesRectangleAndPreservesCollisionQueries) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   component = rectangle_component(rectangle(-20, -20));
	const auto   id        = component->get_id();

	ASSERT_TRUE(panel.add(component));
	ASSERT_TRUE(panel.set_bounds(id, rectangle(20, 20)));

	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(20, 20)).has_value());
	EXPECT_FALSE(panel.find_one_collision_in_area(rectangle(-20, -20)).has_value());
}

TEST(GuiPanel, SetBoundsRejectsUnknownAndOutOfBoundsSlots) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   component = rectangle_component(rectangle(0, 0));
	const auto   id        = component->get_id();
	const auto   unknown   = Components::gen_id(Components::ComponentType::RECTANGLE);

	ASSERT_TRUE(panel.add(component));
	EXPECT_FALSE(panel.set_bounds(unknown, rectangle(0, 0)));
	EXPECT_FALSE(panel.set_bounds(id, rectangle(49, 0, 10, 10)));
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(0, 0)).has_value());
}

TEST(GuiPanel, ResizeRejectsBoundsThatWouldLoseAnIndexedComponent) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   component = rectangle_component(rectangle(35, 0));
	const auto   id        = component->get_id();

	ASSERT_TRUE(panel.add(component));
	EXPECT_FALSE(panel.resize(rectangle(0, 0, 50, 50)));
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(35, 0)).has_value());
	EXPECT_EQ(panel.get(id), component);
}

TEST(GuiPanel, ResizeRebuildsIndexForComponentsThatRemainInside) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   component = rectangle_component(rectangle(10, 10));

	ASSERT_TRUE(panel.add(component));
	ASSERT_TRUE(panel.resize(rectangle(0, 0, 200, 200)));

	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(10, 10)).has_value());
}

TEST(GuiPanel, CircularComponentsUseInscribedSlotsAndExactCollisionFiltering) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   component = circle_component(Geometry::Circle{Geometry::Position{0, 0}, Geometry::Radius{5}});

	ASSERT_TRUE(panel.add(component));
	ASSERT_TRUE(panel.set_bounds(component->get_id(), rectangle(0, 0, 40, 20)));

	EXPECT_TRUE(panel.find_one_collision_in_area(Geometry::Circle{{0, 0}, Geometry::Radius{10}}).has_value());
	EXPECT_FALSE(
		panel.find_one_collision_in_area(Geometry::Rectangle{{13, 13}, Geometry::Width{2}, Geometry::Height{2}})
			.has_value());
}

TEST(GuiPanel, DraggingChangesParentHitTestingWithoutChangingLocalSlots) {
	Panel::Panel panel{rectangle(50, 50, 100, 100)};
	const auto   component = rectangle_component(rectangle(0, 0));

	ASSERT_TRUE(panel.add(component));
	EXPECT_TRUE(panel.collides_in_parent(rectangle(50, 50)).size() == 1);

	panel.drag_by(Geometry::Position{20, 10});

	EXPECT_EQ(panel.origin(), (Geometry::Position{70, 60}));
	EXPECT_TRUE(panel.collides_in_parent(rectangle(70, 60)).size() == 1);
	EXPECT_TRUE(panel.collides(rectangle(0, 0)).size() == 1);
}

TEST(GuiPanel, RelayoutAppliesAssignmentsAndClearsDirtyFlag) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   rectangle    = rectangle_component(::rectangle(-30, 0));
	const auto   rectangle_id = rectangle->get_id();

	ASSERT_TRUE(panel.add(rectangle));
	const auto child = std::make_shared<const Panel::Layout::Node>(
		Panel::Layout::Node{Panel::Layout::Leaf{rectangle_id, [] {
													return Panel::Layout::Size{10, 10};
												}}});
	panel.set_layout(Panel::Layout::Node{Panel::Layout::Stack{.direction   = Panel::Layout::Direction::HORIZONTAL,
															  .cross_align = Panel::Layout::CrossAlign::STRETCH,
															  .children    = {{Panel::Layout::Fixed{20}, child}}}});

	EXPECT_TRUE(panel.relayout().applied);
	EXPECT_FALSE(panel.relayout().applied);
	EXPECT_TRUE(panel.find_one_collision_in_area(::rectangle(-40, 0)).has_value());
	EXPECT_TRUE(panel.find_one_collision_in_area(::rectangle(-30, 0)).has_value());
}
