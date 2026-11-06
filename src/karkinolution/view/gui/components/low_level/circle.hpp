#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/components/base_component.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				class Circle : public BaseComponent<ComponentType::CIRCLE> {
					private:

						Geometry::Circle bounds_;

					public:

						const Karkinolution::Colors::Color color;

						Circle(const Geometry::Circle &bounds, const Karkinolution::Colors::Color &color);

						[[nodiscard]]
						Geometry::Circle get_bounds() const {
							return bounds_;
						}

						bool set_slot(const Geometry::Rectangle &slot);

						void draw() const;
						void draw(const Geometry::Position &origin) const;
						void draw_bounds(Geometry::Thickness thickness) const;
				};
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
