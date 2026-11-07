#include "raylib.h"

#include <opsiamethystos/drivers/font.hpp>
#include <opsiamethystos/gui/components/low_level/text.hpp>
#include <opsiamethystos/raylib_utils/colors.hpp>
#include <stdexcept>
#include <utility>

Opsiamethystos::Gui::Components::LowLevel::Text::Text(
	const Opsiamethystos::Gui::Geometry::Rectangle                          &max_bound,
	std::string                                                              text,
	const KShared::Colors::Color                                            &color,
	GreaterThanZeroValue<int>                                                font_size,
	std::function<Drivers::Font::TextMeasurement(std::string_view, int32_t)> measure)
	: max_bound_(max_bound)
	, current_bound_(max_bound)
	, text_()
	, measure_(std::move(measure))
	, color(color)
	, font_size(font_size) {
	set_text(std::move(text));
}

void Opsiamethystos::Gui::Components::LowLevel::Text::set_text(std::string text) {
	const auto                text_size = measure_(text, font_size.value());
	const Geometry::Rectangle candidate_bound{max_bound_.position,
											  Geometry::Width{text_size.width},
											  Geometry::Height{text_size.height}};

	if (!Geometry::can_contain(max_bound_, candidate_bound)) {
		throw std::invalid_argument("Text does not fit inside its maximum bound");
	}

	text_.swap(text);
	current_bound_ = candidate_bound;
}

bool Opsiamethystos::Gui::Components::LowLevel::Text::set_slot(const Geometry::Rectangle &slot) {
	const auto                text_size = measure_(text_, font_size.value());
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

void Opsiamethystos::Gui::Components::LowLevel::Text::draw() const {
	draw({0, 0});
}

void Opsiamethystos::Gui::Components::LowLevel::Text::draw(const Geometry::Position &origin) const {
	DrawText(text_.c_str(),
			 static_cast<int>(current_bound_.left().value + origin.x),
			 static_cast<int>(current_bound_.top().value + origin.y),
			 font_size.value(),
			 Opsiamethystos::Gui::Colors::translate(color));
}
