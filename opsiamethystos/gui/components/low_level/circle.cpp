#include "opsiamethystos/gui/components/low_level/circle.hpp"

#include "opsiamethystos/raylib_utils/colors.hpp"
#include "raylib.h"

#include <algorithm>

Opsiamethystos::Gui::Components::LowLevel::Circle::Circle(const Opsiamethystos::Gui::Geometry::Circle &bounds,
														  const KShared::Colors::Color                &color)
	: bounds_(bounds)
	, color(color) {}

bool Opsiamethystos::Gui::Components::LowLevel::Circle::set_slot(const Opsiamethystos::Gui::Geometry::Rectangle &slot) {
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

void Opsiamethystos::Gui::Components::LowLevel::Circle::draw() const {
	draw({0, 0});
}

void Opsiamethystos::Gui::Components::LowLevel::Circle::draw(const Geometry::Position &origin) const {
	DrawCircleV(Vector2{bounds_.position.x + origin.x, bounds_.position.y + origin.y},
				bounds_.radius.value(),
				Opsiamethystos::Gui::Colors::translate(color));
}

void Opsiamethystos::Gui::Components::LowLevel::Circle::draw_bounds(
	Opsiamethystos::Gui::Geometry::Thickness thickness) const {
	const Vector2 center{.x = bounds_.position.x, .y = bounds_.position.y};

	const float radius = bounds_.radius.value();

	DrawRing(center,
			 radius - thickness.value(),
			 radius,
			 0.0f,
			 360.0f,
			 36,
			 Opsiamethystos::Gui::Colors::translate(color));
}