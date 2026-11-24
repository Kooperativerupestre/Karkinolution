#include "opsiamethystos/graphics/panel/components/board.hpp"

#include <algorithm>
#include <opsiamethystos/graphics/render/render.hpp>
#include <utility>

namespace Opsiamethystos::Gui::Components {
	bool Board::add(Primitive primitive, const Math::Geometry::Rectangle &primitive_bounds) {
		if (!Math::Geometry::can_contain(bounds_, primitive_bounds)) {
			return false;
		}

		primitives_.push_back(std::move(primitive));
		return true;
	}

	bool Board::draw_circle(const Math::Geometry::Circle &circle, const KShared::Colors::Color &color) {
		const Math::Geometry::Rectangle bounds{
			circle.position,
			Math::Geometry::Width{static_cast<double>(circle.radius.value()) * 2.0},
			Math::Geometry::Height{static_cast<double>(circle.radius.value()) * 2.0}};
		return add(LowLevel::Circle{circle, color}, bounds);
	}

	bool Board::draw_triangle(const Math::Geometry::Triangle &triangle, const KShared::Colors::Color &color) {
		const float left  = std::min({triangle.a.x, triangle.b.x, triangle.c.x});
		const float right = std::max({triangle.a.x, triangle.b.x, triangle.c.x});
		const float top   = std::min({triangle.a.y, triangle.b.y, triangle.c.y});
		const float floor = std::max({triangle.a.y, triangle.b.y, triangle.c.y});
		if (left == right || top == floor) {
			return false;
		}

		const Math::Geometry::Rectangle bounds{Math::Geometry::Position{(left + right) / 2.0F, (top + floor) / 2.0F},
											   Math::Geometry::Width{static_cast<double>(right - left)},
											   Math::Geometry::Height{static_cast<double>(floor - top)}};
		return add(TrianglePrimitive{triangle, color}, bounds);
	}

	bool Board::draw_line(Math::Geometry::Position      begin,
						  Math::Geometry::Position      end,
						  Math::Geometry::Thickness     thickness,
						  const KShared::Colors::Color &color) {
		LowLevel::Line line{begin, end, thickness, color};
		const auto     bounds = line.get_bounds();
		return add(std::move(line), bounds);
	}

	bool Board::draw_rectangle(const Math::Geometry::Rectangle &rectangle, const KShared::Colors::Color &color) {
		return add(LowLevel::Rectangle{rectangle, color}, rectangle);
	}

	bool Board::draw_text(const Math::Geometry::Rectangle &maximum_bounds,
						  std::string                      text,
						  const KShared::Colors::Color    &color,
						  GreaterThanZeroValue<int>        font_size) {
		return add(LowLevel::Text{maximum_bounds, std::move(text), color, font_size}, maximum_bounds);
	}

	void Board::draw(const Math::Geometry::Position &origin) const {
		for (const auto &primitive : primitives_) {
			std::visit(
				[&origin](const auto &drawable) {
					if constexpr (requires { drawable.draw(origin); }) {
						drawable.draw(origin);
					} else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(drawable)>, TrianglePrimitive>) {
						Render::draw_triangle(
							Math::Geometry::Triangle{
								{drawable.triangle.a.x + origin.x, drawable.triangle.a.y + origin.y},
								{drawable.triangle.b.x + origin.x, drawable.triangle.b.y + origin.y},
								{drawable.triangle.c.x + origin.x, drawable.triangle.c.y + origin.y}},
							drawable.color);
					}
				},
				primitive);
		}
	}
} // namespace Opsiamethystos::Gui::Components
