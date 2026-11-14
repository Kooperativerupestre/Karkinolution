#pragma once

#include "opsiamethystos/drivers/font.hpp"
#include "opsiamethystos/drivers/mouse.hpp"
#include "opsiamethystos/gui/components/base_component.hpp"
#include "opsiamethystos/gui/components/identity.hpp"
#include "opsiamethystos/gui/components/low_level/text.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <algorithm>
#include <functional>
#include <opsiamethystos/gui/components/low_level/rectangle.hpp>
#include <string>
#include <string_view>
#include <utility>

namespace Opsiamethystos {
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

							static Geometry::Rectangle fit_label(const Geometry::Rectangle &bounds,
																 std::string_view           label,
																 GreaterThanZeroValue<int>  font_size) {
								constexpr double horizontal_padding = 20.0;
								constexpr double vertical_padding   = 12.0;
								const auto       measured = Drivers::Font::measure_text(label, font_size.value());
								return {bounds.position,
										Geometry::Width{
											std::max(bounds.width.value(), measured.width + horizontal_padding)},
										Geometry::Height{
											std::max(bounds.height.value(), measured.height + vertical_padding)}};
							}

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

							explicit RectangleButton(const Geometry::Rectangle    &bounds,
													 std::function<void()>         callback,
													 ButtonStyles                  style,
													 std::string                   label_text,
													 const KShared::Colors::Color &label_color,
													 GreaterThanZeroValue<int>     font_size)
								: RectangleButton(fit_label(bounds, label_text, font_size),
												  std::move(callback),
												  std::move(style),
												  Text{fit_label(bounds, label_text, font_size),
													   label_text,
													   label_color,
													   font_size}) {}

							explicit RectangleButton(std::function<void()>         callback,
													 ButtonStyles                  style,
													 std::string                   label_text,
													 const KShared::Colors::Color &label_color,
													 GreaterThanZeroValue<int>     font_size)
								: RectangleButton(
									  fit_label(Geometry::Rectangle{{0, 0}, Geometry::Width{1}, Geometry::Height{1}},
												label_text,
												font_size),
									  std::move(callback),
									  std::move(style),
									  std::move(label_text),
									  label_color,
									  font_size) {}

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

							void draw(const Geometry::Position &origin) const {
								draw(ButtonStates::IDLE, origin);
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
} // namespace Opsiamethystos
