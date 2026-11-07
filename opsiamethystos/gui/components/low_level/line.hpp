#pragma once

#include "opsiamethystos/gui/components/base_component.hpp"
#include "opsiamethystos/gui/components/identity.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

namespace Opsiamethystos {
	namespace Gui {
		namespace Components {
			namespace LowLevel {
				class Line : public BaseComponent<ComponentType::LINE> {
					private:

						const Geometry::Position  begin_;
						const Geometry::Position  end_;
						const Geometry::Thickness thickness_;

					public:

						const KShared::Colors::Color color;

						Line(Geometry::Position            begin,
							 Geometry::Position            end,
							 Geometry::Thickness           thickness,
							 const KShared::Colors::Color &color);

						[[nodiscard]]
						Geometry::Rectangle get_bounds() const;

						void draw() const;
						// Lines follow the same accumulated-origin rule as area components, even
						// though their hit-test bound is only a conservative rectangle.
						void draw(const Geometry::Position &origin) const;
				};
			} // namespace LowLevel
		} // namespace Components
	} // namespace Gui
} // namespace Opsiamethystos
