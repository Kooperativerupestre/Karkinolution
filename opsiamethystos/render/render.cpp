#include "opsiamethystos/math/geometry/geometry.hpp"
#include "opsiamethystos/raylib_utils/colors.hpp"
#include "shared/core/color.hpp"
#include "shared/math/stats/compile_values.hpp"

#include <opsiamethystos/render/render.hpp>
#include <raylib.h>

namespace Opsiamethystos {
	void Render::draw_line(const Gui::Geometry::Position  &begin,
						   const Gui::Geometry::Position  &end,
						   const Gui::Geometry::Thickness &thickness,
						   const KShared::Colors::Color   &color) {
		DrawLineEx(Vector2{begin.x, begin.y},
				   Vector2{end.x, end.y},
				   thickness.value(),
				   Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_rectangle(const Gui::Geometry::Rectangle &rectangle, const KShared::Colors::Color &color) {
		DrawRectangle(rectangle.position.x,
					  rectangle.position.y,
					  rectangle.width.value(),
					  rectangle.height.value(),
					  Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_rectangle_lines(const Gui::Geometry::Rectangle &rectangle,
									  const Gui::Geometry::Thickness &thickness,
									  const KShared::Colors::Color   &color) {
		DrawRectangleLinesEx(Rectangle{.x      = rectangle.position.x,
									   .y      = rectangle.position.y,
									   .width  = static_cast<float>(rectangle.width.value()),
									   .height = static_cast<float>(rectangle.height.value())},
							 thickness.value(),
							 Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_rounded_rectangle(const Gui::Geometry::Rectangle       &rectangle,
										const NormalizedValue<float>          roundness,
										const GenericLimitedValue<int, 60, 4> segments,
										const KShared::Colors::Color         &color) {
		DrawRectangleRounded(Rectangle{.x      = rectangle.position.x,
									   .y      = rectangle.position.y,
									   .width  = static_cast<float>(rectangle.width.value()),
									   .height = static_cast<float>(rectangle.height.value())},
							 roundness.value(),
							 segments.value(),
							 Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_rounded_rectangle_lines(const Gui::Geometry::Rectangle       &rectangle,
											  const NormalizedValue<float>          roundness,
											  const GenericLimitedValue<int, 60, 4> segments,
											  const Gui::Geometry::Thickness       &thickness,
											  const KShared::Colors::Color         &color) {
		DrawRectangleRoundedLinesEx(Rectangle{.x      = rectangle.position.x,
											  .y      = rectangle.position.y,
											  .width  = static_cast<float>(rectangle.width.value()),
											  .height = static_cast<float>(rectangle.height.value())},
									roundness.value(),
									segments.value(),
									thickness.value(),
									Gui::Colors::translate(color));
	}

	void Render::draw_circle(const Gui::Geometry::Circle &circle, const KShared::Colors::Color &color) {
		DrawCircle(circle.position.x,
				   circle.position.y,
				   circle.radius.value(),
				   Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_ring(const Gui::Geometry::Radius    &radius,
						   const Gui::Geometry::Position  &position,
						   const Gui::Geometry::Thickness &thickness,
						   const KShared::Colors::Color   &color) {
		DrawRing(Vector2{position.x, position.y},
				 (radius - thickness.value()).value(),
				 radius.value(),
				 0.0f,
				 360.0f,
				 36,
				 Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_ring(const Gui::Geometry::Circle    &circle,
						   const Gui::Geometry::Thickness &thickness,
						   const KShared::Colors::Color   &color) {
		draw_ring(circle.radius, circle.position, thickness, color);
	}

	void Render::draw_triangle(const Gui::Geometry::Triangle &triangle, const KShared::Colors::Color &color) {
		DrawTriangle(Vector2{triangle.a.x, triangle.a.y},
					 Vector2{triangle.b.x, triangle.b.y},
					 Vector2{triangle.c.x, triangle.c.y},
					 Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_triangle_lines(const Gui::Geometry::Triangle  &triangle,
									 const Gui::Geometry::Thickness &thickness,
									 const KShared::Colors::Color   &color) {
		draw_line(triangle.a, triangle.b, thickness, color);
		draw_line(triangle.b, triangle.c, thickness, color);
		draw_line(triangle.c, triangle.a, thickness, color);
	}
} // namespace Opsiamethystos