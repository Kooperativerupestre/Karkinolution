#include "karkinolution/view/gui/components/low_level/circle.hpp"

#include "karkinolution/view/gui/colors.hpp"
#include "raylib.h"

#include <algorithm>

KarkinolutionView::Gui::Components::LowLevel::Circle::Circle(const KarkinolutionView::Gui::Geometry::Circle &bounds,
															 const Karkinolution::Colors::Color             &color)
	: bounds_(bounds)
	, color(color) {}

bool KarkinolutionView::Gui::Components::LowLevel::Circle::set_slot(
	const KarkinolutionView::Gui::Geometry::Rectangle &slot) {
	// Using the smaller extent produces the centered inscribed circle required by
	// the rectangular layout contract and avoids stretching the circle into an oval.
	const float diameter = std::min(static_cast<float>(slot.width.value()), static_cast<float>(slot.height.value()));
	if (diameter <= 0.0F) {
		return false;
	}

	const Geometry::Circle candidate{slot.position, Geometry::Radius{diameter / 2.0F}};
	bounds_ = candidate;
	return true;
}

void KarkinolutionView::Gui::Components::LowLevel::Circle::draw() const {
	draw({0, 0});
}

void KarkinolutionView::Gui::Components::LowLevel::Circle::draw(const Geometry::Position &origin) const {
	DrawCircleV(Vector2{bounds_.position.x + origin.x, bounds_.position.y + origin.y},
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
