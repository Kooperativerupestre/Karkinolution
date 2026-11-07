#include "opsiamethystos/raylib_utils/colors.hpp"

#include <cmath>
#include <opsiamethystos/gui/components/low_level/line.hpp>
#include <raylib.h>

Opsiamethystos::Gui::Components::LowLevel::Line::Line(Opsiamethystos::Gui::Geometry::Position  begin,
													  Opsiamethystos::Gui::Geometry::Position  end,
													  Opsiamethystos::Gui::Geometry::Thickness thickness,
													  const KShared::Colors::Color            &color)
	: begin_(begin)
	, end_(end)
	, thickness_(thickness)
	, color(color) {}

Opsiamethystos::Gui::Geometry::Rectangle Opsiamethystos::Gui::Components::LowLevel::Line::get_bounds() const {
	const float half_thickness = static_cast<float>(thickness_.value()) / 2.0f;
	const float delta_x        = std::abs(end_.x - begin_.x);
	const float delta_y        = std::abs(end_.y - begin_.y);

	return {{(begin_.x + end_.x) / 2.0f, (begin_.y + end_.y) / 2.0f},
			Opsiamethystos::Gui::Geometry::Width{delta_x + 2.0f * half_thickness},
			Opsiamethystos::Gui::Geometry::Height{delta_y + 2.0f * half_thickness}};
}

void Opsiamethystos::Gui::Components::LowLevel::Line::draw() const {
	draw({0, 0});
}

void Opsiamethystos::Gui::Components::LowLevel::Line::draw(const Geometry::Position &origin) const {
	DrawLineEx(Vector2{.x = begin_.x + origin.x, .y = begin_.y + origin.y},
			   Vector2{.x = end_.x + origin.x, .y = end_.y + origin.y},
			   static_cast<float>(thickness_.value()),
			   Colors::translate(color));
}
