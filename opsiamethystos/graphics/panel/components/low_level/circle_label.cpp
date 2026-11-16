#include "opsiamethystos/graphics/panel/components/low_level/circle_label.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace Opsiamethystos::Graphics::Panel::Components {

	namespace {
		Math::Geometry::Rectangle make_text_max_bound(const Math::Geometry::Circle &circle) {
			const auto radius = circle.radius.value();
			if (radius <= 0.0f) {
				throw std::invalid_argument("A circle label requires a positive circle radius");
			}

			const double side = static_cast<double>(radius) * std::sqrt(2.0);
			return {circle.position, Math::Geometry::Width{side}, Math::Geometry::Height{side}};
		}
	} // namespace

	LowLevel::CircleLabel::CircleLabel(const Math::Geometry::Circle &bounds,
									   std::string                   text,
									   const KShared::Colors::Color &circle_color,
									   const KShared::Colors::Color &text_color,
									   GreaterThanZeroValue<int>     font_size)
		: circle_(bounds, circle_color)
		, text_(make_text_max_bound(bounds), std::move(text), text_color, font_size) {}

	void LowLevel::CircleLabel::set_text(std::string text) {
		text_.set_text(std::move(text));
	}

	void LowLevel::CircleLabel::draw() const {
		draw({0, 0});
	}

	void LowLevel::CircleLabel::draw(const Math::Geometry::Position &origin) const {
		circle_.draw(origin);
		text_.draw(origin);
	}

} // namespace Opsiamethystos::Graphics::Panel::Components
