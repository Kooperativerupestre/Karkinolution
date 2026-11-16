#pragma once

#include "opsiamethystos/gui/components/base_component.hpp"
#include "opsiamethystos/gui/components/identity.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

namespace Opsiamethystos {
	namespace Graphics {
		namespace Panel {
			namespace Components {
				namespace LowLevel {
					class Rectangle : public Gui::Components::BaseComponent<Gui::Components::ComponentType::RECTANGLE> {
						private:

							Math::Geometry::Rectangle bounds_;

						public:

							const KShared::Colors::Color color;

							Rectangle(const Math::Geometry::Rectangle &bounds, const KShared::Colors::Color &color);

							[[nodiscard]]
							Math::Geometry::Rectangle get_bounds() const {
								return bounds_;
							}

							// Rectangles interpret the slot directly because their render and hit-test
							// geometry already has the same shape as the layout allocation.
							bool set_slot(const Math::Geometry::Rectangle &slot) {
								bounds_ = slot;
								return true;
							}

							void draw() const;
							// Drawing accepts the accumulated parent origin instead of rewriting the
							// local rectangle during panel movement.
							void draw(const Math::Geometry::Position &origin) const;
							void draw_bounds(Math::Geometry::Thickness thickness) const;
					};
				} // namespace LowLevel
			} // namespace Components
		} // namespace Panel
	} // namespace Graphics
} // namespace Opsiamethystos
