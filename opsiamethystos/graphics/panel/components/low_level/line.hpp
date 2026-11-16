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
					class Line : public Gui::Components::BaseComponent<Gui::Components::ComponentType::LINE> {
						private:

							const Math::Geometry::Position  begin_;
							const Math::Geometry::Position  end_;
							const Math::Geometry::Thickness thickness_;

						public:

							const KShared::Colors::Color color;

							Line(Math::Geometry::Position      begin,
								 Math::Geometry::Position      end,
								 Math::Geometry::Thickness     thickness,
								 const KShared::Colors::Color &color);

							[[nodiscard]]
							Math::Geometry::Rectangle get_bounds() const;

							void draw() const;
							// Lines follow the same accumulated-origin rule as area components, even
							// though their hit-test bound is only a conservative rectangle.
							void draw(const Math::Geometry::Position &origin) const;
					};
				} // namespace LowLevel
			} // namespace Components
		} // namespace Panel
	} // namespace Graphics
} // namespace Opsiamethystos
