#pragma once
#include "opsiamethystos/drivers/font.hpp"
#include "opsiamethystos/gui/components/base_component.hpp"
#include "opsiamethystos/gui/components/identity.hpp"
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
					class Text : public Gui::Components::BaseComponent<Gui::Components::ComponentType::TEXT> {
						private:

							// The stable maximum bound supports spatial indexing. The measured current
							// bound is centered inside it for rendering and hit-testing.
							Math::Geometry::Rectangle max_bound_;
							Math::Geometry::Rectangle current_bound_;
							std::string               text_;
							// Measurement is injectable so layout and rollback tests can use a
							// deterministic provider without creating a Raylib window. Production
							// callers use the Raylib-backed driver by default.
							std::function<Gui::Components::Drivers::Font::TextMeasurement(std::string_view, int32_t)>
								measure_;

						public:

							const KShared::Colors::Color    color;
							const GreaterThanZeroValue<int> font_size;

							// The provider must use the same width/height convention as draw().
							// It is called whenever text is assigned or a panel proposes a new slot.
							Text(const Math::Geometry::Rectangle                                        &max_bound,
								 std::string                                                             text,
								 const KShared::Colors::Color                                           &color,
								 GreaterThanZeroValue<int>                                               font_size,
								 std::function<Gui::Components::Drivers::Font::TextMeasurement(std::string_view,
																							   int32_t)> measure =
									 Gui::Components::Drivers::Font::measure_text);

							[[nodiscard]]
							Math::Geometry::Rectangle get_bounds() const {
								return max_bound_;
							}

							[[nodiscard]]
							Math::Geometry::Rectangle get_current_bounds() const {
								return current_bound_;
							}

							[[nodiscard]]
							const std::string &get_text() const noexcept {
								return text_;
							}

							void set_text(std::string text);
							// A slot changes the stable maximum bound only after the current text
							// fits inside it, allowing Panel to roll back without partial state.
							bool set_slot(const Math::Geometry::Rectangle &slot);
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
