#pragma once
#include "shared/core/color.hpp"
#include "shared/math/stats/compile_values.hpp"

#include <opsiamethystos/graphics/models/models.hpp>
#include <opsiamethystos/math/geometry/geometry.hpp>
#include <string>

namespace Opsiamethystos {
	namespace Render {
		void draw_rectangle(const Math::Geometry::Rectangle &rectangle, const KShared::Colors::Color &color);
		void draw_rectangle_lines(const Math::Geometry::Rectangle &rectangle,
								  const Math::Geometry::Thickness &thickness,
								  const KShared::Colors::Color    &color);

		// [4 ~ 12] is well for segments
		// the minimum is clamped to 4 because is the miminum of raylib
		void draw_rounded_rectangle(const Math::Geometry::Rectangle      &rectangle,
									const NormalizedValue<float>          roundness,
									const GenericLimitedValue<int, 60, 4> segments,
									const KShared::Colors::Color         &color);
		void draw_rounded_rectangle_lines(const Math::Geometry::Rectangle      &rectangle,
										  const NormalizedValue<float>          roundness,
										  const GenericLimitedValue<int, 60, 4> segments,
										  const Math::Geometry::Thickness      &thickness,
										  const KShared::Colors::Color         &color);
		void draw_circle(const Math::Geometry::Circle &circle, const KShared::Colors::Color &color);

		void draw_ring(const Math::Geometry::Radius    &radius,
					   const Math::Geometry::Position  &position,
					   const Math::Geometry::Thickness &thickness,
					   const KShared::Colors::Color    &color);
		void draw_ring(const Math::Geometry::Circle    &circle,
					   const Math::Geometry::Thickness &thickness,
					   const KShared::Colors::Color    &color);
		void draw_line(const Math::Geometry::Position  &begin,
					   const Math::Geometry::Position  &end,
					   const Math::Geometry::Thickness &thickness,
					   const KShared::Colors::Color    &color);
		void draw_triangle(const Math::Geometry::Triangle &triangle, const KShared::Colors::Color &color);
		void draw_triangle_lines(const Math::Geometry::Triangle  &triangle,
								 const Math::Geometry::Thickness &thickness,
								 const KShared::Colors::Color    &color);
		void draw_simple_text(const std::string               &string,
							  const Math::Geometry::Position  &position,
							  const GreaterThanZeroValue<int> &font_size,
							  const KShared::Colors::Color    &color);
		void draw_simple_text(const Graphics::Models::SimpleText &simple_text);
	} // namespace Render
} // namespace Opsiamethystos