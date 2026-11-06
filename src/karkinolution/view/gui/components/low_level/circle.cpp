#include "karkinolution/view/gui/components/low_level/circle.hpp"

#include "karkinolution/view/gui/colors.hpp"
#include "raylib.h"

KarkinolutionView::Gui::Components::LowLevel::Circle::Circle(const KarkinolutionView::Gui::Geometry::Circle &bounds,
															 const Karkinolution::Colors::Color             &color)
	: bounds_(bounds)
	, color(color) {}

void KarkinolutionView::Gui::Components::LowLevel::Circle::draw() const {
	DrawCircleV(Vector2{bounds_.position.x, bounds_.position.y},
				bounds_.radius.value(),
				KarkinolutionView::Gui::Colors::translate(color));
}

void KarkinolutionView::Gui::Components::LowLevel::Circle::draw_bounds(
	KarkinolutionView::Gui::Geometry::Thickness thickness) const {
	DrawCircleLinesEx(Vector2{.x = bounds_.position.x, .y = bounds_.position.y},
					  bounds_.radius.value(),
					  thickness.value(),
					  KarkinolutionView::Gui::Colors::translate(color));
}