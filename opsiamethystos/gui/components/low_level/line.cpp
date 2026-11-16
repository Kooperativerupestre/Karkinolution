#include "opsiamethystos/math/geometry/geometry.hpp"
#include "opsiamethystos/render/render.hpp"

#include <cmath>
#include <opsiamethystos/gui/components/low_level/line.hpp>

Opsiamethystos::Gui::Components::LowLevel::Line::Line(Opsiamethystos::Math::Geometry::Position  begin,
													  Opsiamethystos::Math::Geometry::Position  end,
													  Opsiamethystos::Math::Geometry::Thickness thickness,
													  const KShared::Colors::Color             &color)
	: begin_(begin)
	, end_(end)
	, thickness_(thickness)
	, color(color) {}

Opsiamethystos::Math::Geometry::Rectangle Opsiamethystos::Gui::Components::LowLevel::Line::get_bounds() const {
	const float half_thickness = static_cast<float>(thickness_.value()) / 2.0f;
	const float delta_x        = std::abs(end_.x - begin_.x);
	const float delta_y        = std::abs(end_.y - begin_.y);

	return {{(begin_.x + end_.x) / 2.0f, (begin_.y + end_.y) / 2.0f},
			Opsiamethystos::Math::Geometry::Width{delta_x + 2.0f * half_thickness},
			Opsiamethystos::Math::Geometry::Height{delta_y + 2.0f * half_thickness}};
}

void Opsiamethystos::Gui::Components::LowLevel::Line::draw() const {
	draw({0, 0});
}

void Opsiamethystos::Gui::Components::LowLevel::Line::draw(const Math::Geometry::Position &origin) const {

	Render::draw_line(Math::Geometry::Position{begin_.x + origin.x, begin_.y + origin.y},
					  Math::Geometry::Position{end_.x + origin.x, end_.y + origin.y},
					  thickness_,
					  color);
}
