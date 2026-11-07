#include "opsiamethystos/gui/components/low_level/rectangle.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <opsiamethystos/gui/components/panel.hpp>

void Opsiamethystos::Gui::Components::Panel::draw_bounds(Opsiamethystos::Gui::Geometry::Thickness thickness) const {
	form_.draw_bounds(thickness);
}

void Opsiamethystos::Gui::Components::Panel::fill_background(const KShared::Colors::Color &color) const {
	Opsiamethystos::Gui::Components::LowLevel::Rectangle{form_.get_bounds(), color}.draw();
}