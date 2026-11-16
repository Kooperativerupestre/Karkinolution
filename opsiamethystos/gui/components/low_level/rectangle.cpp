#include "opsiamethystos/math/geometry/geometry.hpp"

#include <opsiamethystos/gui/components/low_level/rectangle.hpp>
#include <opsiamethystos/raylib_utils/colors.hpp>
#include <opsiamethystos/render/render.hpp>

Opsiamethystos::Gui::Components::LowLevel::Rectangle::Rectangle(const Opsiamethystos::Math::Geometry::Rectangle &bounds,
																const KShared::Colors::Color                    &color)
	: bounds_(bounds)
	, color(color) {}

void Opsiamethystos::Gui::Components::LowLevel::Rectangle::draw() const {
	draw({0, 0});
}

void Opsiamethystos::Gui::Components::LowLevel::Rectangle::draw(const Math::Geometry::Position &origin) const {

	Render::draw_rectangle(Math::Geometry::Rectangle{Math::Geometry::Position{bounds_.left().value + origin.x,
																			  bounds_.top().value + origin.y},
													 bounds_.width,
													 bounds_.height},
						   color);
}

void Opsiamethystos::Gui::Components::LowLevel::Rectangle::draw_bounds(
	Opsiamethystos::Math::Geometry::Thickness thickness) const {
	Render::draw_rectangle_lines(bounds_, thickness, color);
}
