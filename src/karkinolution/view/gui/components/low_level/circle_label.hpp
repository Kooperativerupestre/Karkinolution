#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/components/base_component.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/components/low_level/circle.hpp"
#include "karkinolution/view/gui/components/low_level/text.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <string>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				class CircleLabel : public BaseComponent<ComponentType::CIRCLE_LABEL> {
					private:

						const Circle circle_;
						Text         text_;

					public:

						CircleLabel(const Geometry::Circle             &bounds,
									std::string                         text,
									const Karkinolution::Colors::Color &circle_color,
									const Karkinolution::Colors::Color &text_color,
									GreaterThanZeroValue<int>           font_size);

						[[nodiscard]]
						Geometry::Circle get_bounds() const {
							return circle_.get_bounds();
						}

						[[nodiscard]]
						Geometry::Rectangle get_current_bounds() const {
							return text_.get_current_bounds();
						}

						[[nodiscard]]
						const std::string &get_text() const noexcept {
							return text_.get_text();
						}

						void set_text(std::string text);
						void draw() const;
				};
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
