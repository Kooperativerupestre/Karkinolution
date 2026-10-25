#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			class Rectangle {
				public:

					Geometry::Rectangle          value;
					Karkinolution::Colors::Color color;
					void                         draw() const;
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView