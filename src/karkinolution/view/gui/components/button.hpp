#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <optional>
#include <string>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			class ButtonStyle {
				public:

					Karkinolution::Colors::Color idle;
					Karkinolution::Colors::Color hovered;
					Karkinolution::Colors::Color pressed;
					Karkinolution::Colors::Color text;

					// Derives hovered and pressed from the idle color.
					static constexpr ButtonStyle
					from_base(const Karkinolution::Colors::Color &base,
							  const Karkinolution::Colors::Color &text) {
						constexpr NormalizedValue<float> hover_lighten{0.15f};
						constexpr NormalizedValue<float> press_darken{0.8f};

						return {.idle    = base,
								.hovered = base.lighten(hover_lighten),
								.pressed = base.multiply_without_alpha(press_darken),
								.text    = text};
					}
			};

			class Button {
				public:

					std::string                     label;
					ButtonStyle                     style;
					std::optional<Geometry::Width>  width;
					std::optional<Geometry::Height> height;

					// Returns true on the frame the button is clicked.
					[[nodiscard]] bool draw() const;
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView