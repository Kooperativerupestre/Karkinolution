#pragma once

#include "shared/core/color.hpp"
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

						const KShared::Colors::Color color;

						Circle(const Geometry::Circle &bounds, const KShared::Colors::Color &color);

						[[nodiscard]]
						Geometry::Circle get_bounds() const {
							return bounds_;
						}

						// Layout remains rectangular, so the circle adopts the largest centered
						// circle that fits the slot. Shape-specific fitting stays out of Layout.
						bool set_slot(const Geometry::Rectangle &slot);

						void draw() const;
						// The origin translates panel-local geometry at the rendering boundary;
						// the stored circle remains local and is not mutated by drawing.
						void draw(const Geometry::Position &origin) const;
						void draw_bounds(Geometry::Thickness thickness) const;
				};
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
