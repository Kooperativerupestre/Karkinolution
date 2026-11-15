#include "opsiamethystos/gui/components/board.hpp"

#include <utility>

namespace Opsiamethystos::Gui::Components {
	bool Board::add(Primitive primitive, const Geometry::Rectangle &primitive_bounds) {
		if (!Geometry::can_contain(bounds_, primitive_bounds)) {
			return false;
		}

		primitives_.push_back(std::move(primitive));
		return true;
	}

	bool Board::draw_circle(const Geometry::Circle &circle, const KShared::Colors::Color &color) {
		const Geometry::Rectangle bounds{circle.position,
										 Geometry::Width{static_cast<double>(circle.radius.value()) * 2.0},
										 Geometry::Height{static_cast<double>(circle.radius.value()) * 2.0}};
		return add(LowLevel::Circle{circle, color}, bounds);
	}

	bool Board::draw_line(Geometry::Position            begin,
						  Geometry::Position            end,
						  Geometry::Thickness           thickness,
						  const KShared::Colors::Color &color) {
		LowLevel::Line line{begin, end, thickness, color};
		const auto     bounds = line.get_bounds();
		return add(std::move(line), bounds);
	}

	bool Board::draw_rectangle(const Geometry::Rectangle &rectangle, const KShared::Colors::Color &color) {
		return add(LowLevel::Rectangle{rectangle, color}, rectangle);
	}

	bool Board::draw_text(const Geometry::Rectangle    &maximum_bounds,
						  std::string                   text,
						  const KShared::Colors::Color &color,
						  GreaterThanZeroValue<int>     font_size) {
		return add(LowLevel::Text{maximum_bounds, std::move(text), color, font_size}, maximum_bounds);
	}

	void Board::draw(const Geometry::Position &origin) const {
		for (const auto &primitive : primitives_) {
			std::visit(
				[&origin](const auto &drawable) {
					drawable.draw(origin);
				},
				primitive);
		}
	}
} // namespace Opsiamethystos::Gui::Components
