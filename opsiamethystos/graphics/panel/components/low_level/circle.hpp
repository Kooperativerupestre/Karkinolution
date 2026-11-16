#pragma once

#include "opsiamethystos/graphics/panel/components/base_component.hpp"
#include "opsiamethystos/graphics/panel/components/identity.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

namespace Opsiamethystos {
	namespace Graphics {
		namespace Panel {
			namespace Components {
				namespace LowLevel {
					class Circle : public Gui::Components::BaseComponent<Gui::Components::ComponentType::CIRCLE> {
						private:

							Math::Geometry::Circle bounds_;

						public:

							const KShared::Colors::Color color;

							Circle(const Math::Geometry::Circle &bounds, const KShared::Colors::Color &color);

							[[nodiscard]]
							Math::Geometry::Circle get_bounds() const {
								return bounds_;
							}

							// Layout remains rectangular, so the circle adopts the largest centered
							// circle that fits the slot. Shape-specific fitting stays out of Layout.
							bool set_slot(const Math::Geometry::Rectangle &slot);

							void draw() const;
							// The origin translates panel-local geometry at the rendering boundary;
							// the stored circle remains local and is not mutated by drawing.
							void draw(const Math::Geometry::Position &origin) const;
							void draw_bounds(Math::Geometry::Thickness thickness) const;
					};
				} // namespace LowLevel
			} // namespace Components
		} // namespace Panel
	} // namespace Graphics
} // namespace Opsiamethystos
