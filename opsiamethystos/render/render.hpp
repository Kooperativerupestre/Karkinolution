#pragma once
#include "shared/core/color.hpp"
#include "shared/math/stats/compile_values.hpp"

#include <opsiamethystos/math/geometry/geometry.hpp>

namespace Opsiamethystos {
	namespace Render {
		void draw_rectangle(const Gui::Geometry::Rectangle &rectangle, const KShared::Colors::Color &color);
		void draw_rectangle_lines(const Gui::Geometry::Rectangle &rectangle,
								  const Gui::Geometry::Thickness &thickness,
								  const KShared::Colors::Color   &color);

		// [4 ~ 12] is well for segments
		// the minimum is clamped to 4 because is the miminum of raylib
		void draw_rounded_rectangle(const Gui::Geometry::Rectangle       &rectangle,
									const NormalizedValue<float>          roundness,
									const GenericLimitedValue<int, 60, 4> segments,
									const KShared::Colors::Color         &color);
		void draw_rounded_rectangle_lines(const Gui::Geometry::Rectangle       &rectangle,
										  const NormalizedValue<float>          roundness,
										  const GenericLimitedValue<int, 60, 4> segments,
										  const Gui::Geometry::Thickness       &thickness,
										  const KShared::Colors::Color         &color);
		void draw_circle(const Gui::Geometry::Circle &circle, const KShared::Colors::Color &color);

		void draw_ring(const Gui::Geometry::Radius    &radius,
					   const Gui::Geometry::Position  &position,
					   const Gui::Geometry::Thickness &thickness,
					   const KShared::Colors::Color   &color);
		void draw_ring(const Gui::Geometry::Circle    &circle,
					   const Gui::Geometry::Thickness &thickness,
					   const KShared::Colors::Color   &color);
		void draw_line(const Gui::Geometry::Position  &begin,
					   const Gui::Geometry::Position  &end,
					   const Gui::Geometry::Thickness &thickness,
					   const KShared::Colors::Color   &color);
		void draw_triangle(const Gui::Geometry::Triangle &triangle, const KShared::Colors::Color &color);
		void draw_triangle_lines(const Gui::Geometry::Triangle  &triangle,
								 const Gui::Geometry::Thickness &thickness,
								 const KShared::Colors::Color   &color);
	} // namespace Render
} // namespace Opsiamethystos