#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/components/base_component.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/components/low_level/rectangle.hpp"
#include "karkinolution/view/gui/components/low_level/text.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <string>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				class RectangleLabel : public BaseComponent<ComponentType::RECTANGLE_LABEL> {
					private:

						Rectangle rectangle_;
						Text      text_;

					public:

						RectangleLabel(const Geometry::Rectangle          &bounds,
									   std::string                         text,
									   const Karkinolution::Colors::Color &rectangle_color,
									   const Karkinolution::Colors::Color &text_color,
									   GreaterThanZeroValue<int>           font_size);

						[[nodiscard]]
						Geometry::Rectangle get_bounds() const {
							return rectangle_.get_bounds();
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

						// Both children must accept the same slot. Restoring the rectangle when
						// text rejects it keeps the compound component atomic.
						bool set_slot(const Geometry::Rectangle &slot) {
							const auto old_slot = rectangle_.get_bounds();
							if (!rectangle_.set_slot(slot)) {
								return false;
							}
							if (!text_.set_slot(slot)) {
								rectangle_.set_slot(old_slot);
								return false;
							}
							return true;
						}

						void draw() const;
						// The origin is forwarded to both children so their local geometry stays
						// unchanged while the panel moves in its parent.
						void draw(const Geometry::Position &origin) const;
				};
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
