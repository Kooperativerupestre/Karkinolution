#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/gui/components/low_level/rectangle.hpp>
#include <raylib.h>

KarkinolutionView::Gui::Components::LowLevel::Rectangle::Rectangle(
	const KarkinolutionView::Gui::Geometry::Rectangle &bounds,
	const Karkinolution::Colors::Color                &color)
	: bounds_(bounds)
	, color(color) {}

void KarkinolutionView::Gui::Components::LowLevel::Rectangle::draw() const {
	DrawRectangleRec(::Rectangle{.x      = bounds_.left().value,
								 .y      = bounds_.top().value,
								 .width  = static_cast<float>(bounds_.width.value()),
								 .height = static_cast<float>(bounds_.height.value())},
					 KarkinolutionView::Gui::Colors::translate(color));
}

void KarkinolutionView::Gui::Components::LowLevel::Rectangle::draw_bounds(
	KarkinolutionView::Gui::Geometry::Thickness thickness) const {
	DrawRectangleLinesEx(::Rectangle{.x      = bounds_.left().value,
									 .y      = bounds_.top().value,
									 .width  = static_cast<float>(bounds_.width.value()),
									 .height = static_cast<float>(bounds_.height.value())},
						 thickness.value(),
						 KarkinolutionView::Gui::Colors::translate(color));
}