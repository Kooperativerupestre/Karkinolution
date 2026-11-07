#include "opsiamethystos/math/geometry/geometry.hpp"

#include <opsiamethystos/gui/components/low_level/rectangle.hpp>
#include <opsiamethystos/raylib_utils/colors.hpp>
#include <raylib.h>

Opsiamethystos::Gui::Components::LowLevel::Rectangle::Rectangle(const Opsiamethystos::Gui::Geometry::Rectangle &bounds,
																const KShared::Colors::Color                   &color)
	: bounds_(bounds)
	, color(color) {}

void Opsiamethystos::Gui::Components::LowLevel::Rectangle::draw() const {
	draw({0, 0});
}

void Opsiamethystos::Gui::Components::LowLevel::Rectangle::draw(const Geometry::Position &origin) const {
	DrawRectangleRec(::Rectangle{.x      = bounds_.left().value + origin.x,
								 .y      = bounds_.top().value + origin.y,
								 .width  = static_cast<float>(bounds_.width.value()),
								 .height = static_cast<float>(bounds_.height.value())},
					 Opsiamethystos::Gui::Colors::translate(color));
}

void Opsiamethystos::Gui::Components::LowLevel::Rectangle::draw_bounds(
	Opsiamethystos::Gui::Geometry::Thickness thickness) const {
	DrawRectangleLinesEx(::Rectangle{.x      = bounds_.left().value,
									 .y      = bounds_.top().value,
									 .width  = static_cast<float>(bounds_.width.value()),
									 .height = static_cast<float>(bounds_.height.value())},
						 thickness.value(),
						 Opsiamethystos::Gui::Colors::translate(color));
}
