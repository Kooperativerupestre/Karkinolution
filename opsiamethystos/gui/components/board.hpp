#pragma once

#include "opsiamethystos/gui/components/base_component.hpp"
#include "opsiamethystos/gui/components/low_level/circle.hpp"
#include "opsiamethystos/gui/components/low_level/line.hpp"
#include "opsiamethystos/gui/components/low_level/rectangle.hpp"
#include "opsiamethystos/gui/components/low_level/text.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <string>
#include <variant>
#include <vector>

namespace Opsiamethystos::Gui::Components {
	// Board is a free-positioning drawing surface. It owns primitive draw
	// commands rather than layout components and never exposes the renderer in
	// its public interface.
	class Board : public BaseComponent<ComponentType::BOARD> {
		private:

			using Primitive = std::variant<LowLevel::Circle, LowLevel::Line, LowLevel::Rectangle, LowLevel::Text>;

			Geometry::Rectangle    bounds_;
			std::vector<Primitive> primitives_;

			// The primitive must be fully contained before it enters the command list.
			// A failed insertion leaves the board unchanged.
			bool add(Primitive primitive, const Geometry::Rectangle &primitive_bounds);

		public:

			// Containment includes the boundary, so primitives may touch the edge but
			// may not render any part of their geometry outside this rectangle.
			explicit Board(const Geometry::Rectangle &bounds)
				: bounds_(bounds) {}

			[[nodiscard]] Geometry::Rectangle get_bounds() const noexcept {
				return bounds_;
			}

			// Positions are supplied by the caller; Board does not rearrange them.
			bool draw_circle(const Geometry::Circle &circle, const KShared::Colors::Color &color);
			bool draw_line(Geometry::Position            begin,
						   Geometry::Position            end,
						   Geometry::Thickness           thickness,
						   const KShared::Colors::Color &color);
			bool draw_rectangle(const Geometry::Rectangle &rectangle, const KShared::Colors::Color &color);
			bool draw_text(const Geometry::Rectangle    &maximum_bounds,
						   std::string                   text,
						   const KShared::Colors::Color &color,
						   GreaterThanZeroValue<int>     font_size);

			[[nodiscard]] std::size_t size() const noexcept {
				return primitives_.size();
			}

			// Drawing follows insertion order; later primitives appear above earlier ones.
			void draw(const Geometry::Position &origin = {0, 0}) const;
	};
} // namespace Opsiamethystos::Gui::Components
