#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/view/gui/components/base_component.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				class Rectangle : public BaseComponent<ComponentType::RECTANGLE> {
					private:

						Geometry::Rectangle bounds_;

					public:

						const Karkinolution::Colors::Color color;

						Rectangle(const Geometry::Rectangle &bounds, const Karkinolution::Colors::Color &color);

						[[nodiscard]]
						Geometry::Rectangle get_bounds() const {
							return bounds_;
						}

						bool set_slot(const Geometry::Rectangle &slot) {
							bounds_ = slot;
							return true;
						}

						void draw() const;
						void draw(const Geometry::Position &origin) const;
						void draw_bounds(Geometry::Thickness thickness) const;
				};
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
