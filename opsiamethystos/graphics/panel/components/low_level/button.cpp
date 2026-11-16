#include "opsiamethystos/graphics/render/render.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"

#include <opsiamethystos/drivers/mouse.hpp>
#include <opsiamethystos/graphics/panel/components/low_level/button.hpp>

namespace Opsiamethystos::Graphics::Panel::Components {

	void LowLevel::Button::RectangleButton::draw(LowLevel::Button::ButtonStates  state,
												 const Math::Geometry::Position &origin) const {
		const auto bounds = rectangle_component_.get_bounds();

		Render::draw_rectangle(Math::Geometry::Rectangle{Math::Geometry::Position{bounds.left().value + origin.x,
																				  bounds.top().value + origin.y},
														 bounds.width,
														 bounds.height},
							   style_.get(state));


		label.draw_centered_in(bounds, origin);
	}

	void LowLevel::Button::RectangleButton::run() {
		run({0, 0});
	}

	void LowLevel::Button::RectangleButton::run(const Math::Geometry::Position &origin) {
		// Mouse coordinates are in parent/screen space while button bounds are
		// local. Translating the bound for the query keeps interaction aligned
		// with the translated rendering without mutating layout state.
		const auto                      bounds = rectangle_component_.get_bounds();
		const Math::Geometry::Rectangle screen_bounds{bounds.position + origin, bounds.width, bounds.height};
		const bool                      hovered = Gui::Components::Drivers::Mouse::is_cursor_inside(screen_bounds);
		const bool                      pressed =
			hovered && Gui::Components::Drivers::Mouse::is_pressed(Gui::Components::Drivers::Mouse::Buttons::LEFT);
		if (Gui::Components::Drivers::Mouse::was_pressed(Gui::Components::Drivers::Mouse::Buttons::LEFT)) {
			press_started_inside_ = hovered;
		}

		draw(pressed       ? LowLevel::Button::ButtonStates::PRESSED
				 : hovered ? LowLevel::Button::ButtonStates::HOVERED
						   : LowLevel::Button::ButtonStates::IDLE,
			 origin);

		if (Gui::Components::Drivers::Mouse::was_released(Gui::Components::Drivers::Mouse::Buttons::LEFT)) {
			const bool clicked    = hovered && press_started_inside_;
			press_started_inside_ = false;
			if (clicked) {
				invoke();
			}
		}
	}

} // namespace Opsiamethystos::Graphics::Panel::Components
