#pragma once
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <karkinolution/core/color.hpp>
#include <string>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			class Text {
				public:

					std::string                                text;
					Karkinolution::Colors::Color               color;
					KarkinolutionView::Gui::Geometry::Position position;
					GreaterThanZeroValue<int>                  font_size;

					void draw() const;
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView