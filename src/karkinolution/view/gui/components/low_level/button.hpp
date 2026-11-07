#pragma once

#include "shared/core/color.hpp"
#include "shared/functional/unreachable_or_throw.hpp"
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

							KShared::Colors::Color idle, hovered, pressed;

						public:

							ButtonStyles(const KShared::Colors::Color &idle,
										 const KShared::Colors::Color &hovered,
										 const KShared::Colors::Color &pressed)
								: idle(idle)
								, hovered(hovered)
								, pressed(pressed) {}

							const KShared::Colors::Color &operator[](ButtonStates state) const {
								switch (state) {
									case ButtonStates::IDLE:
										return idle;

									case ButtonStates::HOVERED:
										return hovered;
									case ButtonStates::PRESSED:
										return pressed;
									default:
										KShared::Functional::unreachable_or_throw();
								}
							}

							const KShared::Colors::Color &get(ButtonStates state) const {
								return (*this)[state];
							}
					};

					class RectangleButton : public BaseComponent<ComponentType::BUTTON> {
						private:

							Rectangle             rectangle_component_;
							std::function<void()> callback_;
							ButtonStyles          style_;
							bool                  press_started_inside_ = false;

							void draw(ButtonStates state, const Geometry::Position &origin) const;

							void invoke() const {
								std::invoke(callback_);
							}

							Text label;

						public:

							explicit RectangleButton(const Geometry::Rectangle &bounds,
													 std::function<void()>      callback,
													 ButtonStyles               style,
													 const Text                &text)
								: rectangle_component_(bounds, style.get(ButtonStates::IDLE))
								, callback_(std::move(callback))
								, style_(std::move(style))
								, label(text) {}

							[[nodiscard]]
							Geometry::Rectangle get_bounds() const {
								return rectangle_component_.get_bounds();
							}

							void set_text(std::string text) {
								label.set_text(std::move(text));
							}

							// A button is a rectangle plus a label, so slot changes must be atomic
							// across both parts to avoid drawing a label outside its background.
							bool set_slot(const Geometry::Rectangle &slot) {
								const auto old_slot = rectangle_component_.get_bounds();
								if (!rectangle_component_.set_slot(slot)) {
									return false;
								}
								if (!label.set_slot(slot)) {
									rectangle_component_.set_slot(old_slot);
									return false;
								}
								return true;
							}

							void run();
							// Input and drawing use the same parent-space translation; press state is
							// still tracked in the button and is not part of layout geometry.
							void run(const Geometry::Position &origin);
					};
				} // namespace Button
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
