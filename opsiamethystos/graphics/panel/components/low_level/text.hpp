#pragma once
#include "opsiamethystos/drivers/font.hpp"
#include "opsiamethystos/graphics/panel/components/base_component.hpp"
#include "opsiamethystos/graphics/panel/components/identity.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"

#include <functional>
#include <shared/core/color.hpp>
#include <string>
#include <string_view>

namespace Opsiamethystos {
	namespace Graphics {
		namespace Panel {
			namespace Components {
				namespace LowLevel {
					// A line of text placed in a slot. The text never rejects its slot, and changing the
					// text never fails for lack of space: the text is measured and centered on the slot, so
					// a text larger than its slot overflows evenly on both sides. What the text can do
					// therefore never depends on the slot of an earlier layout pass.
					//
					// Future: overflow is drawn as is. Clipping it to the slot with a scissor region
					// (Raylib's BeginScissorMode) would be a useful mechanic; the slot is already stored
					// here, so only the draw path and the render layer would need to learn about it.
					class Text : public Gui::Components::BaseComponent<Gui::Components::ComponentType::TEXT> {
						private:

							// The slot supports spatial indexing and hit-testing. The measured current
							// bound is centered on it for rendering and may be larger than the slot.
							Math::Geometry::Rectangle slot_;
							Math::Geometry::Rectangle current_bound_;
							std::string               text_;
							// Measurement is injectable so layout and rollback tests can use a
							// deterministic provider without creating a Raylib window. Production
							// callers use the Raylib-backed driver by default.
							std::function<Gui::Components::Drivers::Font::TextMeasurement(std::string_view, int32_t)>
								measure_;

							// The size of `text` centered on `center`. Throws when the text cannot be
							// measured as a non-empty extent, before any state has changed.
							[[nodiscard]]
							Math::Geometry::Rectangle measured_bound(std::string_view                text,
																	 const Math::Geometry::Position &center) const;

						public:

							const KShared::Colors::Color    color;
							const GreaterThanZeroValue<int> font_size;

							// The provider must use the same width/height convention as draw().
							// It is called whenever text is assigned or a panel proposes a new slot.
							Text(const Math::Geometry::Rectangle                                        &slot,
								 std::string                                                             text,
								 const KShared::Colors::Color                                           &color,
								 GreaterThanZeroValue<int>                                               font_size,
								 std::function<Gui::Components::Drivers::Font::TextMeasurement(std::string_view,
																							   int32_t)> measure =
									 Gui::Components::Drivers::Font::measure_text);

							[[nodiscard]]
							Math::Geometry::Rectangle get_bounds() const {
								return slot_;
							}

							[[nodiscard]]
							Math::Geometry::Rectangle get_current_bounds() const {
								return current_bound_;
							}

							[[nodiscard]]
							const std::string &get_text() const noexcept {
								return text_;
							}

							// Replaces the text and re-measures it. Does not depend on the slot, so it
							// does not throw for a text that is larger than the slot.
							void set_text(std::string text);
							// Moves the text to a new slot. Always accepts and returns true; the return
							// type only keeps Text composable with components that can refuse a slot.
							bool set_slot(const Math::Geometry::Rectangle &slot);

							void set_color(KShared::Colors::Color color) {
								color = std::move(color);
							}

							void draw() const;
							// Text is measured and stored locally; origin is added only for rendering
							// so indexed/current bounds remain stable during panel dragging.
							void draw(const Math::Geometry::Position &origin) const;
							void draw_centered_in(const Math::Geometry::Rectangle &container,
												  const Math::Geometry::Position  &origin = {0, 0}) const;
					};
				} // namespace LowLevel
			} // namespace Components
		} // namespace Panel
	} // namespace Graphics
} // namespace Opsiamethystos
