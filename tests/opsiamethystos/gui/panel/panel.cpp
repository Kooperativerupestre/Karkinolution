#include <gtest/gtest.h>
#include <memory>
#include <opsiamethystos/graphics/panel/components/component.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/circle.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/line.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/rectangle.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/text.hpp>
#include <opsiamethystos/graphics/panel/panel.hpp>
#include <shared/core/color.hpp>
#include <string_view>

namespace {
	namespace Geometry   = Opsiamethystos::Math::Geometry;
	namespace Components = Opsiamethystos::Gui::Components;
	namespace Drivers    = Components::Drivers;
	namespace LowLevel   = Components::LowLevel;
	namespace Panel      = Opsiamethystos::Gui::Panel;
	using KShared::Colors::Presets::White;

	Geometry::Rectangle rectangle(float x, float y, double width = 10.0, double height = 10.0) {
		return {Geometry::Position{x, y}, Geometry::Width{width}, Geometry::Height{height}};
	}

	std::shared_ptr<Components::Component> rectangle_component(const Geometry::Rectangle &bounds) {
		return std::make_shared<Components::Component>(Components::AllComponents{LowLevel::Rectangle{bounds, White}});
	}

	std::shared_ptr<Components::Component> circle_component(const Geometry::Circle &bounds) {
		return std::make_shared<Components::Component>(Components::AllComponents{LowLevel::Circle{bounds, White}});
	}

	// A line has no rectangular slot, so it is the component that always refuses one.
	std::shared_ptr<Components::Component> line_component(const Geometry::Position &begin,
														  const Geometry::Position &end) {
		return std::make_shared<Components::Component>(
			Components::AllComponents{LowLevel::Line{begin, end, Geometry::Thickness{1}, White}});
	}

	std::shared_ptr<Components::Component> text_component(const Geometry::Rectangle &bounds) {
		const auto measure = [](const std::string_view, const int32_t) {
			return Drivers::Font::TextMeasurement{20.0F, 10.0F};
		};
		return std::make_shared<Components::Component>(Components::AllComponents{
			LowLevel::Text{bounds, "rollback", White, KShared::Math::LV::CT::GreaterThanZeroValue<int>{10}, measure}});
	}

	std::shared_ptr<const Panel::Layout::Node> leaf_node(const Components::ComponentId &id) {
		return std::make_shared<const Panel::Layout::Node>(
			Panel::Layout::Node{Panel::Layout::Leaf{id, [] {
														return Panel::Layout::Size{10, 10};
													}}});
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

TEST(GuiPanel, SetBoundsRollsBackWhenTheComponentRejectsItsSlot) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   component  = line_component(Geometry::Position{-5, 0}, Geometry::Position{5, 0});
	const auto   id         = component->get_id();
	const auto   old_bounds = component->get_bounds();

	ASSERT_TRUE(panel.add(component));
	EXPECT_FALSE(panel.set_bounds(id, rectangle(20, 20, 4, 4)));

	EXPECT_EQ(std::get<Geometry::Rectangle>(component->get_bounds()), std::get<Geometry::Rectangle>(old_bounds));
	EXPECT_EQ(panel.collides(rectangle(0, 0, 2, 2)).size(), 1);
	EXPECT_TRUE(panel.collides(rectangle(20, 20, 4, 4)).empty());
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

TEST(GuiPanel, ResizeKeepsBoundsLocalAndMovesTheParentOrigin) {
	Panel::Panel panel{rectangle(50, 50, 100, 100)};
	const auto   component = rectangle_component(rectangle(0, 0));

	ASSERT_TRUE(panel.add(component));
	ASSERT_TRUE(panel.resize(rectangle(100, 80, 200, 120)));

	EXPECT_EQ(panel.origin(), (Geometry::Position{100, 80}));
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(0, 0)).has_value());
	EXPECT_TRUE(panel.find_one_collision_in_parent(rectangle(100, 80)).has_value());
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
	EXPECT_TRUE(panel.find_one_collision_in_parent(rectangle(70, 60)).has_value());
}

TEST(GuiPanel, UpdateAppliesAssignmentsAndDoesNothingWhenNothingChanged) {
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

	const auto first_pass  = panel.update();
	const auto second_pass = panel.update();
	ASSERT_TRUE(first_pass.has_value());
	ASSERT_TRUE(second_pass.has_value());
	EXPECT_TRUE(first_pass->applied);
	EXPECT_FALSE(second_pass->applied);
	EXPECT_TRUE(panel.find_one_collision_in_area(::rectangle(-40, 0)).has_value());
	EXPECT_TRUE(panel.find_one_collision_in_area(::rectangle(-30, 0)).has_value());
}

TEST(GuiPanel, UpdateRollsBackEarlierAssignmentsWhenALaterComponentRejects) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   rectangle_component_ptr = rectangle_component(rectangle(-30, 0));
	const auto   line_component_ptr      = line_component(Geometry::Position{0, 10}, Geometry::Position{20, 10});
	const auto   rectangle_id            = rectangle_component_ptr->get_id();
	const auto   line_id                 = line_component_ptr->get_id();

	ASSERT_TRUE(panel.add(rectangle_component_ptr));
	ASSERT_TRUE(panel.add(line_component_ptr));

	const auto rectangle_child = std::make_shared<const Panel::Layout::Node>(
		Panel::Layout::Node{Panel::Layout::Leaf{rectangle_id, [] {
													return Panel::Layout::Size{10, 10};
												}}});
	const auto line_child = std::make_shared<const Panel::Layout::Node>(
		Panel::Layout::Node{Panel::Layout::Leaf{line_id, [] {
													return Panel::Layout::Size{10, 10};
												}}});
	panel.set_layout(Panel::Layout::Node{Panel::Layout::Stack{
		.direction = Panel::Layout::Direction::HORIZONTAL,
		.children  = {{Panel::Layout::Fixed{20}, rectangle_child}, {Panel::Layout::Fixed{1}, line_child}}}});

	const auto result = panel.update();

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error().code, Panel::LayoutErrorCode::COMPONENT_REJECTED_SLOT);
	EXPECT_EQ(result.error().component, line_id);
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(-30, 0)).has_value());
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(10, 10, 21, 1)).has_value());
}

TEST(GuiPanel, UpdateReportsAnUnknownComponentReferencedByTheLayout) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   unknown = Components::gen_id(Components::ComponentType::RECTANGLE);

	panel.set_layout(
		Panel::Layout::Node{Panel::Layout::Stack{.direction = Panel::Layout::Direction::HORIZONTAL,
												 .children  = {{Panel::Layout::Fixed{20}, leaf_node(unknown)}}}});

	const auto result = panel.update();

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error().code, Panel::LayoutErrorCode::UNKNOWN_COMPONENT);
	EXPECT_EQ(result.error().component, unknown);
}

TEST(GuiPanel, UpdateReportsAComponentRemovedAfterSetLayoutAndMovesNothing) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   kept    = rectangle_component(rectangle(-30, 0));
	const auto   removed = rectangle_component(rectangle(30, 0));

	ASSERT_TRUE(panel.add(kept));
	ASSERT_TRUE(panel.add(removed));
	panel.set_layout(Panel::Layout::Node{
		Panel::Layout::Stack{.direction = Panel::Layout::Direction::HORIZONTAL,
							 .children  = {{Panel::Layout::Fixed{20}, leaf_node(kept->get_id())},
										   {Panel::Layout::Fixed{20}, leaf_node(removed->get_id())}}}});
	ASSERT_TRUE(panel.remove(removed->get_id()));

	const auto result = panel.update();

	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error().code, Panel::LayoutErrorCode::UNKNOWN_COMPONENT);
	EXPECT_EQ(result.error().component, removed->get_id());
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(-30, 0)).has_value());
}

TEST(GuiPanel, UpdateWithoutALayoutIsNotAnErrorAndMovesNothing) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};

	ASSERT_TRUE(panel.add(rectangle_component(rectangle(-30, 0))));
	const auto result = panel.update();

	ASSERT_TRUE(result.has_value());
	EXPECT_FALSE(result->applied);
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(-30, 0)).has_value());
}

TEST(GuiPanel, FailedUpdateKeepsFailingUntilTheLayoutIsFixed) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   rectangle_ptr = rectangle_component(rectangle(30, 0));
	const auto   line_ptr      = line_component(Geometry::Position{0, 10}, Geometry::Position{20, 10});

	ASSERT_TRUE(panel.add(rectangle_ptr));
	ASSERT_TRUE(panel.add(line_ptr));
	panel.set_layout(Panel::Layout::row({Panel::Layout::fixed(20, rectangle_ptr), Panel::Layout::fixed(20, line_ptr)}));

	for (int frame = 0; frame < 2; ++frame) {
		const auto result = panel.update();
		ASSERT_FALSE(result.has_value());
		EXPECT_EQ(result.error().code, Panel::LayoutErrorCode::COMPONENT_REJECTED_SLOT);
		EXPECT_EQ(result.error().component, line_ptr->get_id());
	}

	panel.set_layout(Panel::Layout::row({Panel::Layout::fixed(20, rectangle_ptr)}));
	const auto fixed = panel.update();

	ASSERT_TRUE(fixed.has_value());
	EXPECT_TRUE(fixed->applied);
}

TEST(GuiPanel, UpdateReportsOverflowWhileStillCommittingTheClampedSlots) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   first  = rectangle_component(rectangle(-30, 0));
	const auto   second = rectangle_component(rectangle(30, 0));

	ASSERT_TRUE(panel.add(first));
	ASSERT_TRUE(panel.add(second));
	panel.set_layout(Panel::Layout::Node{
		Panel::Layout::Stack{.direction = Panel::Layout::Direction::HORIZONTAL,
							 .children  = {{Panel::Layout::Fixed{80}, leaf_node(first->get_id())},
										   {Panel::Layout::Fixed{80}, leaf_node(second->get_id())}}}});

	const auto result = panel.update();

	ASSERT_TRUE(result.has_value());
	EXPECT_TRUE(result->applied);
	EXPECT_TRUE(result->overflowed);
}

TEST(GuiPanel, SetLayoutRejectsAnInvalidTreeAndKeepsThePreviousLayout) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   component = rectangle_component(rectangle(30, 0));

	ASSERT_TRUE(panel.add(component));
	panel.set_layout(Panel::Layout::Node{
		Panel::Layout::Stack{.direction   = Panel::Layout::Direction::HORIZONTAL,
							 .cross_align = Panel::Layout::CrossAlign::STRETCH,
							 .children    = {{Panel::Layout::Fixed{20}, leaf_node(component->get_id())}}}});

	EXPECT_THROW(panel.set_layout(Panel::Layout::Node{
					 Panel::Layout::Stack{.direction = Panel::Layout::Direction::HORIZONTAL,
										  .children  = {{Panel::Layout::Fixed{-1}, leaf_node(component->get_id())}}}}),
				 std::invalid_argument);

	const auto result = panel.update();
	ASSERT_TRUE(result.has_value());
	EXPECT_TRUE(result->applied);
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(-40, 0)).has_value());
}

TEST(GuiPanel, UpdateAppliesAPendingLayoutOnceAndThenDoesNothing) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   component = rectangle_component(rectangle(30, 0));

	ASSERT_TRUE(panel.add(component));
	panel.set_layout(Panel::Layout::row({Panel::Layout::fixed(20, component)}));

	const auto first  = panel.update();
	const auto second = panel.update();

	ASSERT_TRUE(first.has_value());
	ASSERT_TRUE(second.has_value());
	EXPECT_TRUE(first->applied);
	EXPECT_FALSE(second->applied);
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(-40, 0)).has_value());
}

TEST(GuiPanel, UpdateKeepsReportingAnErrorUntilTheMissingComponentIsAdded) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   late = rectangle_component(rectangle(30, 0));

	panel.set_layout(Panel::Layout::row({Panel::Layout::fixed(20, late)}));

	for (int frame = 0; frame < 3; ++frame) {
		const auto result = panel.update();
		ASSERT_FALSE(result.has_value());
		EXPECT_EQ(result.error().code, Panel::LayoutErrorCode::UNKNOWN_COMPONENT);
		EXPECT_EQ(result.error().component, late->get_id());
	}

	ASSERT_TRUE(panel.add(late));
	const auto recovered = panel.update();

	ASSERT_TRUE(recovered.has_value());
	EXPECT_TRUE(recovered->applied);
	EXPECT_TRUE(panel.find_one_collision_in_area(rectangle(-40, 0)).has_value());
}

// Content slots equal the text's measured size, so a slot that exactly fits must be accepted.
TEST(GuiPanel, UpdateAcceptsAContentSlotThatFitsTheTextExactly) {
	Panel::Panel panel{rectangle(0, 0, 100, 100)};
	const auto   text = text_component(rectangle(0, 0, 80, 30));

	ASSERT_TRUE(panel.add(text));
	panel.set_layout(
		Panel::Layout::row({Panel::Layout::content(text)}, {.cross_align = Panel::Layout::CrossAlign::START}));

	const auto result = panel.update();

	ASSERT_TRUE(result.has_value());
	EXPECT_TRUE(result->applied);
}
