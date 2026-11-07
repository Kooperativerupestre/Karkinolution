#include "shared/core/color.hpp"
#include "karkinolution/view/gui/components/low_level/rectangle.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <karkinolution/view/gui/components/panel.hpp>

void KarkinolutionView::Gui::Components::Panel::draw_bounds(
	KarkinolutionView::Gui::Geometry::Thickness thickness) const {
	form_.draw_bounds(thickness);
}

void KarkinolutionView::Gui::Components::Panel::fill_background(const KShared::Colors::Color &color) const {
	KarkinolutionView::Gui::Components::LowLevel::Rectangle{form_.get_bounds(), color}.draw();
}