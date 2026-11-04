#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/functional/unreachable_or_throw.hpp"
#include "karkinolution/view/gui/components/base_component.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/components/low_level/text.hpp"
#include "karkinolution/view/gui/drivers/mouse.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <functional>
#include <karkinolution/view/gui/components/low_level/rectangle.hpp>
#include <string>
#include <utility>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				namespace Button {
					enum class ButtonStates : uint8_t {
						IDLE,
						HOVERED,
						PRESSED
					};

					class ButtonStyles {
						private:

							Karkinolution::Colors::Color idle, hovered, pressed;

						public:

							ButtonStyles(const Karkinolution::Colors::Color &idle,
										 const Karkinolution::Colors::Color &hovered,
										 const Karkinolution::Colors::Color &pressed)
								: idle(idle)
								, hovered(hovered)
								, pressed(pressed) {}

							const Karkinolution::Colors::Color &
							operator[](ButtonStates state) const {
								switch (state) {
									case ButtonStates::IDLE:
										return idle;

									case ButtonStates::HOVERED:
										return hovered;
									case ButtonStates::PRESSED:
										return pressed;
									default:
										Karkinolution::Functional::unreachable_or_throw();
								}
							}

							const Karkinolution::Colors::Color &get(ButtonStates state) const {
								return (*this)[state];
							}
					};

					class RectangleButton : public BaseComponent<ComponentType::BUTTON> {
						private:

							const Rectangle       rectangle_component_;
							std::function<void()> callback_;
							ButtonStyles          style_;


							void draw_rectangle_component() const;
							void draw() const;

							void invoke() const {
								std::invoke(callback_);
							}

							bool is_being_touched() const {
								return Drivers::Mouse::is_cursor_inside(
										   rectangle_component_.get_bounds())
									&& Drivers::Mouse::is_pressed(Drivers::Mouse::Buttons::LEFT);
							}

							Text label;

						public:

							explicit RectangleButton(const Rectangle      &bounds,
													 std::function<void()> callback,
													 ButtonStyles          style,
													 const Text           &text)
								: rectangle_component_(bounds)
								, callback_(std::move(callback))
								, style_(style)
								, label(text) {}

							[[nodiscard]]
							Geometry::Rectangle get_bounds() const {
								return rectangle_component_.get_bounds();
							}

							void run();
					};
				} // namespace Button
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView