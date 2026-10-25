#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/gui/components/rectangle.hpp>
#include <raylib.h>

void KarkinolutionView::Gui::Components::Rectangle::draw() const {
	DrawRectangle(value.position.x,
				  value.position.y,
				  value.width.value(),
				  value.height.value(),
				  KarkinolutionView::Gui::Colors::translate(color));
}