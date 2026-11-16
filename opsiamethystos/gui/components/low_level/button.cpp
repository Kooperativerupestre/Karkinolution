#include "opsiamethystos/math/geometry/geometry.hpp"
#include "opsiamethystos/render/render.hpp"

#include <opsiamethystos/drivers/mouse.hpp>
#include <opsiamethystos/gui/components/low_level/button.hpp>

namespace Opsiamethystos {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				void Button::RectangleButton::draw(Button::ButtonStates            state,
												   const Math::Geometry::Position &origin) const {
					const auto bounds = rectangle_component_.get_bounds();

					Render::draw_rectangle(
						Math::Geometry::Rectangle{
							Math::Geometry::Position{bounds.left().value + origin.x, bounds.top().value + origin.y},
							bounds.width,
							bounds.height},
						style_.get(state));


					label.draw_centered_in(bounds, origin);
				}

				void Button::RectangleButton::run() {
					run({0, 0});
				}

				void Button::RectangleButton::run(const Math::Geometry::Position &origin) {
					// Mouse coordinates are in parent/screen space while button bounds are
					// local. Translating the bound for the query keeps interaction aligned
					// with the translated rendering without mutating layout state.
					const auto                      bounds = rectangle_component_.get_bounds();
					const Math::Geometry::Rectangle screen_bounds{bounds.position + origin,
																  bounds.width,
																  bounds.height};
					const bool                      hovered = Drivers::Mouse::is_cursor_inside(screen_bounds);
					const bool pressed = hovered && Drivers::Mouse::is_pressed(Drivers::Mouse::Buttons::LEFT);
					if (Drivers::Mouse::was_pressed(Drivers::Mouse::Buttons::LEFT)) {
						press_started_inside_ = hovered;
					}

					draw(pressed       ? Button::ButtonStates::PRESSED
							 : hovered ? Button::ButtonStates::HOVERED
									   : Button::ButtonStates::IDLE,
						 origin);

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
} // namespace Opsiamethystos
