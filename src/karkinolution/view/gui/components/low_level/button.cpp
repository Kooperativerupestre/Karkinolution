#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/gui/components/low_level/button.hpp>
#include <raylib.h>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				void Button::RectangleButton::draw(Button::ButtonStates state) const {
					const auto bounds = rectangle_component_.get_bounds();
					DrawRectangleRec(
						::Rectangle{.x      = bounds.left().value,
									.y      = bounds.top().value,
									.width  = static_cast<float>(bounds.width.value()),
									.height = static_cast<float>(bounds.height.value())},
						Colors::translate(style_.get(state)));
					label.draw();
				}

				void Button::RectangleButton::run() {
					const bool hovered =
						Drivers::Mouse::is_cursor_inside(rectangle_component_.get_bounds());
					const bool pressed =
						hovered && Drivers::Mouse::is_pressed(Drivers::Mouse::Buttons::LEFT);
					if (Drivers::Mouse::was_pressed(Drivers::Mouse::Buttons::LEFT)) {
						press_started_inside_ = hovered;
					}

					draw(pressed       ? Button::ButtonStates::PRESSED
							 : hovered ? Button::ButtonStates::HOVERED
									   : Button::ButtonStates::IDLE);

					if (Drivers::Mouse::was_released(Drivers::Mouse::Buttons::LEFT)) {
						const bool clicked    = hovered && press_started_inside_;
						press_started_inside_ = false;
						if (clicked) {
							invoke();
						}
					}
				}
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView