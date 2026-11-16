#include "opsiamethystos/math/geometry/geometry.hpp"
#include "opsiamethystos/raylib_utils/colors.hpp"
#include "shared/core/color.hpp"
#include "shared/math/stats/compile_values.hpp"

#include <opsiamethystos/graphics/render/render.hpp>
#include <raylib.h>

namespace Opsiamethystos {
	void Render::draw_line(const Math::Geometry::Position  &begin,
						   const Math::Geometry::Position  &end,
						   const Math::Geometry::Thickness &thickness,
						   const KShared::Colors::Color    &color) {
		DrawLineEx(Vector2{begin.x, begin.y},
				   Vector2{end.x, end.y},
				   thickness.value(),
				   Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_rectangle(const Math::Geometry::Rectangle &rectangle, const KShared::Colors::Color &color) {
		DrawRectangle(rectangle.position.x,
					  rectangle.position.y,
					  rectangle.width.value(),
					  rectangle.height.value(),
					  Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_rectangle_lines(const Math::Geometry::Rectangle &rectangle,
									  const Math::Geometry::Thickness &thickness,
									  const KShared::Colors::Color    &color) {
		DrawRectangleLinesEx(Rectangle{.x      = rectangle.position.x,
									   .y      = rectangle.position.y,
									   .width  = static_cast<float>(rectangle.width.value()),
									   .height = static_cast<float>(rectangle.height.value())},
							 thickness.value(),
							 Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_rounded_rectangle(const Math::Geometry::Rectangle      &rectangle,
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

	void Render::draw_rounded_rectangle_lines(const Math::Geometry::Rectangle      &rectangle,
											  const NormalizedValue<float>          roundness,
											  const GenericLimitedValue<int, 60, 4> segments,
											  const Math::Geometry::Thickness      &thickness,
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

	void Render::draw_circle(const Math::Geometry::Circle &circle, const KShared::Colors::Color &color) {
		DrawCircle(circle.position.x,
				   circle.position.y,
				   circle.radius.value(),
				   Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_ring(const Math::Geometry::Radius    &radius,
						   const Math::Geometry::Position  &position,
						   const Math::Geometry::Thickness &thickness,
						   const KShared::Colors::Color    &color) {
		DrawRing(Vector2{position.x, position.y},
				 (radius - thickness.value()).value(),
				 radius.value(),
				 0.0f,
				 360.0f,
				 36,
				 Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_ring(const Math::Geometry::Circle    &circle,
						   const Math::Geometry::Thickness &thickness,
						   const KShared::Colors::Color    &color) {
		draw_ring(circle.radius, circle.position, thickness, color);
	}

	void Render::draw_triangle(const Math::Geometry::Triangle &triangle, const KShared::Colors::Color &color) {
		DrawTriangle(Vector2{triangle.a.x, triangle.a.y},
					 Vector2{triangle.b.x, triangle.b.y},
					 Vector2{triangle.c.x, triangle.c.y},
					 Opsiamethystos::Gui::Colors::translate(color));
	}

	void Render::draw_triangle_lines(const Math::Geometry::Triangle  &triangle,
									 const Math::Geometry::Thickness &thickness,
									 const KShared::Colors::Color    &color) {
		draw_line(triangle.a, triangle.b, thickness, color);
		draw_line(triangle.b, triangle.c, thickness, color);
		draw_line(triangle.c, triangle.a, thickness, color);
	}

	void Render::draw_simple_text(const std::string               &string,
								  const Math::Geometry::Position  &position,
								  const GreaterThanZeroValue<int> &font_size,
								  const KShared::Colors::Color    &color) {
		DrawText(string.c_str(), position.x, position.y, font_size.value(), Gui::Colors::translate(color));
	}

	void Render::draw_simple_text(const Graphics::Models::SimpleText &simple_text) {
		draw_simple_text(simple_text.text, simple_text.position, simple_text.font_size, simple_text.color);
	}
} // namespace Opsiamethystos