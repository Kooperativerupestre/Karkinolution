#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <karkinolution/view/gui/components/low_level/button.hpp>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				void Button::RectangleButton::draw_rectangle_component() const {
					rectangle_component_.draw_bounds(Geometry::Thickness{2});
				}

				void Button::RectangleButton::draw() const {
					draw_rectangle_component();
				}
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView