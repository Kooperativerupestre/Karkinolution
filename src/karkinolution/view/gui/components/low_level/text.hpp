#pragma once
#include "karkinolution/view/gui/components/base_component.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/drivers/font.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <functional>
#include <karkinolution/core/color.hpp>
#include <string>
#include <string_view>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				class Text : public BaseComponent<ComponentType::TEXT> {
					private:

						// The stable maximum bound supports spatial indexing. The measured current
						// bound is centered inside it for rendering and hit-testing.
						Geometry::Rectangle max_bound_;
						Geometry::Rectangle current_bound_;
						std::string         text_;
						// Measurement is injectable so layout and rollback tests can use a
						// deterministic provider without creating a Raylib window. Production
						// callers use the Raylib-backed driver by default.
						std::function<Drivers::Font::TextMeasurement(std::string_view, int32_t)> measure_;

					public:

						const Karkinolution::Colors::Color color;
						const GreaterThanZeroValue<int>    font_size;

						// The provider must use the same width/height convention as draw().
						// It is called whenever text is assigned or a panel proposes a new slot.
						Text(const Geometry::Rectangle                                               &max_bound,
							 std::string                                                              text,
							 const Karkinolution::Colors::Color                                      &color,
							 GreaterThanZeroValue<int>                                                font_size,
							 std::function<Drivers::Font::TextMeasurement(std::string_view, int32_t)> measure =
								 Drivers::Font::measure_text);

						[[nodiscard]]
						Geometry::Rectangle get_bounds() const {
							return max_bound_;
						}

						[[nodiscard]]
						Geometry::Rectangle get_current_bounds() const {
							return current_bound_;
						}

						[[nodiscard]]
						const std::string &get_text() const noexcept {
							return text_;
						}

						void set_text(std::string text);
						bool set_slot(const Geometry::Rectangle &slot);
						void draw() const;
						void draw(const Geometry::Position &origin) const;
				};
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
