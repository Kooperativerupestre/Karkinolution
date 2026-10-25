#include "karkinolution/view/gui/colors.hpp"

#include <karkinolution/view/gui/components/line.hpp>
#include <raylib.h>

void KarkinolutionView::Gui::Components::Line::draw() const {
	DrawLineEx(Vector2{.x = static_cast<float>(begin.x), .y = static_cast<float>(begin.y)},
			   Vector2{.x = static_cast<float>(end.x), .y = static_cast<float>(end.y)},
			   static_cast<float>(thickness.value()),
			   Colors::translate(color));
}