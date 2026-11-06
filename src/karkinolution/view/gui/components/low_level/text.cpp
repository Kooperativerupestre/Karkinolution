#include "raylib.h"

#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/gui/components/low_level/text.hpp>
#include <karkinolution/view/gui/drivers/font.hpp>
#include <stdexcept>
#include <utility>

KarkinolutionView::Gui::Components::LowLevel::Text::Text(const KarkinolutionView::Gui::Geometry::Rectangle &max_bound,
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
	const auto                text_size = Drivers::Font::measure_text(text, font_size.value());
	const Geometry::Rectangle candidate_bound{max_bound_.position,
											  Geometry::Width{text_size.width},
											  Geometry::Height{text_size.height}};

	if (!Geometry::can_contain(max_bound_, candidate_bound)) {
		throw std::invalid_argument("Text does not fit inside its maximum bound");
	}

	text_.swap(text);
	current_bound_ = candidate_bound;
}

bool KarkinolutionView::Gui::Components::LowLevel::Text::set_slot(const Geometry::Rectangle &slot) {
	const auto                text_size = Drivers::Font::measure_text(text_, font_size.value());
	const Geometry::Rectangle candidate_bound{slot.position,
											  Geometry::Width{text_size.width},
											  Geometry::Height{text_size.height}};
	if (!Geometry::can_contain(slot, candidate_bound)) {
		return false;
	}

	max_bound_     = slot;
	current_bound_ = candidate_bound;
	return true;
}

void KarkinolutionView::Gui::Components::LowLevel::Text::draw() const {
	draw({0, 0});
}

void KarkinolutionView::Gui::Components::LowLevel::Text::draw(const Geometry::Position &origin) const {
	DrawText(text_.c_str(),
			 static_cast<int>(current_bound_.left().value + origin.x),
			 static_cast<int>(current_bound_.top().value + origin.y),
			 font_size.value(),
			 KarkinolutionView::Gui::Colors::translate(color));
}
