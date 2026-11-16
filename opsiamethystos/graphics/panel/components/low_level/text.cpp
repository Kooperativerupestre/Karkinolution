#include "opsiamethystos/math/geometry/geometry.hpp"
#include "opsiamethystos/render/render.hpp"

#include <opsiamethystos/drivers/font.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/text.hpp>
#include <opsiamethystos/raylib_utils/colors.hpp>
#include <stdexcept>
#include <utility>

namespace Opsiamethystos::Graphics::Panel::Components {

	LowLevel::Text::Text(
		const Opsiamethystos::Math::Geometry::Rectangle                                          &max_bound,
		std::string                                                                               text,
		const KShared::Colors::Color                                                             &color,
		GreaterThanZeroValue<int>                                                                 font_size,
		std::function<Gui::Components::Drivers::Font::TextMeasurement(std::string_view, int32_t)> measure)
		: max_bound_(max_bound)
		, current_bound_(max_bound)
		, text_()
		, measure_(std::move(measure))
		, color(color)
		, font_size(font_size) {
		set_text(std::move(text));
	}

	void LowLevel::Text::set_text(std::string text) {
		const auto                      text_size = measure_(text, font_size.value());
		const Math::Geometry::Rectangle candidate_bound{max_bound_.position,
														Math::Geometry::Width{text_size.width},
														Math::Geometry::Height{text_size.height}};

		if (!Math::Geometry::can_contain(max_bound_, candidate_bound)) {
			throw std::invalid_argument("Text does not fit inside its maximum bound");
		}

		text_.swap(text);
		current_bound_ = candidate_bound;
	}

	bool LowLevel::Text::set_slot(const Math::Geometry::Rectangle &slot) {
		const auto                      text_size = measure_(text_, font_size.value());
		const Math::Geometry::Rectangle candidate_bound{slot.position,
														Math::Geometry::Width{text_size.width},
														Math::Geometry::Height{text_size.height}};
		if (!Math::Geometry::can_contain(slot, candidate_bound)) {
			return false;
		}

		max_bound_     = slot;
		current_bound_ = candidate_bound;
		return true;
	}

	void LowLevel::Text::draw() const {
		draw({0, 0});
	}

	void LowLevel::Text::draw(const Math::Geometry::Position &origin) const {
		Render::draw_simple_text(text_,
								 Math::Geometry::Position{static_cast<float>(current_bound_.left().value + origin.x),
														  static_cast<float>(current_bound_.top().value + origin.y)},
								 font_size,
								 color);
	}

	void LowLevel::Text::draw_centered_in(const Math::Geometry::Rectangle &container,
										  const Math::Geometry::Position  &origin) const {
		const auto measured = measure_(text_, font_size.value());

		Render::draw_simple_text(
			text_,
			Math::Geometry::Position{static_cast<float>(container.position.x - measured.width / 2.0 + origin.x),
									 static_cast<float>(container.position.y - measured.height / 2.0 + origin.y)},
			font_size,
			color);
	}

} // namespace Opsiamethystos::Graphics::Panel::Components
