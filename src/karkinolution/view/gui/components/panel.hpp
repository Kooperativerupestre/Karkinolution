#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/components/low_level/circle.hpp"
#include "karkinolution/view/gui/components/low_level/rectangle.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			class Panel {
				private:

					const LowLevel::Rectangle form_;

				public:

					explicit Panel(const LowLevel::Rectangle &form)
						: form_(form) {}

					[[nodiscard]]
					bool can_contain(const LowLevel::Rectangle &component) const {
						return Geometry::can_contain(form_.get_bounds(), component.get_bounds());
					}

					[[nodiscard]]
					bool can_contain(const LowLevel::Circle &component) const {
						return Geometry::can_contain(form_.get_bounds(), component.get_bounds());
					}

					void draw_bounds(Geometry::Thickness thickness) const;
					void fill_background(const Karkinolution::Colors::Color &color) const;
			};
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView