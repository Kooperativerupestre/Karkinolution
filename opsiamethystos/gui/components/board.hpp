#pragma once

#include "opsiamethystos/graphics/panel/components/low_level/circle.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/line.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/rectangle.hpp"
#include "opsiamethystos/graphics/panel/components/low_level/text.hpp"
#include "opsiamethystos/gui/components/base_component.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"

#include <string>
#include <variant>
#include <vector>

namespace Opsiamethystos::Gui::Components {
	namespace LowLevel = Graphics::Panel::Components::LowLevel;

	// Board is a free-positioning drawing surface. It owns primitive draw
	// commands rather than layout components and never exposes the renderer in
	// its public interface.
	class Board : public BaseComponent<ComponentType::BOARD> {
		private:

			using Primitive = std::variant<LowLevel::Circle, LowLevel::Line, LowLevel::Rectangle, LowLevel::Text>;

			Math::Geometry::Rectangle bounds_;
			std::vector<Primitive>    primitives_;

			// The primitive must be fully contained before it enters the command list.
			// A failed insertion leaves the board unchanged.
			bool add(Primitive primitive, const Math::Geometry::Rectangle &primitive_bounds);

		public:

			// Containment includes the boundary, so primitives may touch the edge but
			// may not render any part of their geometry outside this rectangle.
			explicit Board(const Math::Geometry::Rectangle &bounds)
				: bounds_(bounds) {}

			[[nodiscard]] Math::Geometry::Rectangle get_bounds() const noexcept {
				return bounds_;
			}

			// Positions are supplied by the caller; Board does not rearrange them.
			bool draw_circle(const Math::Geometry::Circle &circle, const KShared::Colors::Color &color);
			bool draw_line(Math::Geometry::Position      begin,
						   Math::Geometry::Position      end,
						   Math::Geometry::Thickness     thickness,
						   const KShared::Colors::Color &color);
			bool draw_rectangle(const Math::Geometry::Rectangle &rectangle, const KShared::Colors::Color &color);
			bool draw_text(const Math::Geometry::Rectangle &maximum_bounds,
						   std::string                      text,
						   const KShared::Colors::Color    &color,
						   GreaterThanZeroValue<int>        font_size);

			[[nodiscard]] std::size_t size() const noexcept {
				return primitives_.size();
			}

			// Drawing follows insertion order; later primitives appear above earlier ones.
			void draw(const Math::Geometry::Position &origin = {0, 0}) const;
	};
} // namespace Opsiamethystos::Gui::Components
