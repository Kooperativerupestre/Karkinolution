#include "karkinolution/view/gui/geometry/geometry.hpp"
#include "raylib.h"

#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/gui/components/low_level/text.hpp>
#include <stdexcept>
#include <utility>

KarkinolutionView::Gui::Components::LowLevel::Text::Text(
	const KarkinolutionView::Gui::Geometry::Rectangle &max_bound,
	std::string                                        text,
	const Karkinolution::Colors::Color                &color,
	GreaterThanZeroValue<int>                          font_size)
	: max_bound_(max_bound)
	, current_bound_(max_bound)
	, text_()
	, color(color)
	, font_size(font_size) {
	set_text(std::move(text));
}

void KarkinolutionView::Gui::Components::LowLevel::Text::set_text(std::string text) {
	const auto text_size =
		MeasureTextEx(GetFontDefault(), text.c_str(), static_cast<float>(font_size.value()), 1.0F);
	const Geometry::Rectangle candidate_bound{max_bound_.position,
											  Geometry::Width{text_size.x},
											  Geometry::Height{text_size.y}};

	if (!Geometry::can_contain(max_bound_, candidate_bound)) {
		throw std::invalid_argument("Text does not fit inside its maximum bound");
	}

	text_.swap(text);
	current_bound_ = candidate_bound;
}

void KarkinolutionView::Gui::Components::LowLevel::Text::draw() const {
	DrawText(text_.c_str(),
			 current_bound_.left().value,
			 current_bound_.top().value,
			 font_size.value(),
			 KarkinolutionView::Gui::Colors::translate(color));
}