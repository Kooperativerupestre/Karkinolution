#include "karkinolution/view/gui/components/low_level/circle_label.hpp"

#include <cmath>
#include <stdexcept>
#include <utility>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				namespace {
					Geometry::Rectangle make_text_max_bound(const Geometry::Circle &circle) {
						const auto radius = circle.radius.value();
						if (radius <= 0.0f) {
							throw std::invalid_argument("A circle label requires a positive circle radius");
						}

						const double side = static_cast<double>(radius) * std::sqrt(2.0);
						return {circle.position, Geometry::Width{side}, Geometry::Height{side}};
					}
				} // namespace

				CircleLabel::CircleLabel(const Geometry::Circle             &bounds,
										 std::string                         text,
										 const Karkinolution::Colors::Color &circle_color,
										 const Karkinolution::Colors::Color &text_color,
										 GreaterThanZeroValue<int>           font_size)
					: circle_(bounds, circle_color)
					, text_(make_text_max_bound(bounds), std::move(text), text_color, font_size) {}

				void CircleLabel::set_text(std::string text) {
					text_.set_text(std::move(text));
				}

				void CircleLabel::draw() const {
					circle_.draw();
					text_.draw();
				}
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
