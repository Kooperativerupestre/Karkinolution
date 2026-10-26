#include "raylib.h"

#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/gui/components/text.hpp>

void KarkinolutionView::Gui::Components::Text::draw() const {
	DrawText(text.c_str(),
			 position.x,
			 position.y,
			 font_size.value(),
			 KarkinolutionView::Gui::Colors::translate(color));
}