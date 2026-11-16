#pragma once

#include <cstdint>
#include <opsiamethystos/math/geometry/geometry.hpp>

namespace Opsiamethystos {
	namespace Gui {
		namespace Components {
			namespace Drivers {
				namespace Mouse {

					enum class Buttons : uint8_t {
						LEFT,
						RIGHT,
						MIDDLE
					};
					Math::Geometry::Position get_cursor_position();

					bool is_cursor_inside(const Math::Geometry::Position  &cursor_position,
										  const Math::Geometry::Rectangle &rectangle);
					bool is_cursor_inside(const Math::Geometry::Position &cursor_position,
										  const Math::Geometry::Circle   &circle);

					bool is_cursor_inside(const Math::Geometry::Rectangle &rectangle);
					bool is_cursor_inside(const Math::Geometry::Circle &circle);

					enum class ButtonEvents : uint8_t {
						IS_PRESSED,
						IS_NOT_PRESSED,
						WAS_RELEASED,
						WAS_PRESSED
					};

					bool is_pressed(Buttons button);
					bool is_not_pressed(Buttons button);
					bool was_released(Buttons button);
					bool was_pressed(Buttons button);

					ButtonEvents get_button_event(Buttons button);

				}; // namespace Mouse
			} // namespace Drivers
		} // namespace Components
	} // namespace Gui
} // namespace Opsiamethystos