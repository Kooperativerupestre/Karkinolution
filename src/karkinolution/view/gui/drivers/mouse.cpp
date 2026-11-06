#pragma once
#include "karkinolution/functional/unreachable_or_throw.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"
#include "raylib.h"

#include <karkinolution/view/gui/drivers/mouse.hpp>
#include <utility>

using KMouseButton = KarkinolutionView::Gui::Components::Drivers::Mouse::Buttons;

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace Drivers {
				constexpr MouseButton button_mouse_to_raylib(KMouseButton button) {
					switch (button) {
						case KMouseButton::LEFT:
							return MOUSE_BUTTON_LEFT;

						case KMouseButton::MIDDLE:
							return MOUSE_BUTTON_MIDDLE;

						case KMouseButton::RIGHT:
							return MOUSE_BUTTON_RIGHT;
					}

					std::unreachable();
				}

				Geometry::Position Mouse::get_cursor_position() {
					const auto p = GetMousePosition();
					return {p.x, p.y};
				}

				bool Mouse::is_cursor_inside(const Gui::Geometry::Position &cursor_position,
											 const Geometry::Rectangle     &rectangle) {
					return Geometry::can_contain(rectangle, cursor_position);
				}

				bool Mouse::is_cursor_inside(const Gui::Geometry::Position &cursor_position,
											 const Geometry::Circle        &circle) {
					return Geometry::can_contain(circle, cursor_position);
				}

				bool Mouse::is_cursor_inside(const Geometry::Rectangle &rectangle) {
					return is_cursor_inside(get_cursor_position(), rectangle);
				}

				bool Mouse::is_cursor_inside(const Gui::Geometry::Circle &circle) {
					return is_cursor_inside(get_cursor_position(), circle);
				}

				bool Mouse::is_pressed(KMouseButton button) {
					return IsMouseButtonDown(button_mouse_to_raylib(button));
				}

				bool Mouse::is_not_pressed(KMouseButton button) {
					return IsMouseButtonUp(button_mouse_to_raylib(button));
				}

				bool Mouse::was_released(KMouseButton button) {
					return IsMouseButtonReleased(button_mouse_to_raylib(button));
				}

				bool Mouse::was_pressed(KMouseButton button) {
					return IsMouseButtonPressed(button_mouse_to_raylib(button));
				}

				Mouse::ButtonEvents Mouse::get_button_event(KMouseButton button) {
					if (was_released(button)) {
						return ButtonEvents::WAS_RELEASED;
					} else if (was_pressed(button)) {
						return ButtonEvents::WAS_PRESSED;
					} else if (is_pressed(button)) {
						return ButtonEvents::IS_PRESSED;
					} else if (is_not_pressed(button)) {
						return ButtonEvents::IS_NOT_PRESSED;
					}
					Karkinolution::Functional::unreachable_or_throw("Invalid state of a button event");
				}

			} // namespace Drivers
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView