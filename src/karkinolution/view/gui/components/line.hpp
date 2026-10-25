#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			class Line {
				public:

					Geometry::Position           begin, end;
					Geometry::Thickness          thickness;
					Karkinolution::Colors::Color color;

					void draw() const;
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView