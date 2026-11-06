#include "karkinolution/view/gui/colors.hpp"

#include <cmath>
#include <karkinolution/view/gui/components/low_level/line.hpp>
#include <raylib.h>

KarkinolutionView::Gui::Components::LowLevel::Line::Line(KarkinolutionView::Gui::Geometry::Position  begin,
														 KarkinolutionView::Gui::Geometry::Position  end,
														 KarkinolutionView::Gui::Geometry::Thickness thickness,
														 const Karkinolution::Colors::Color         &color)
	: begin_(begin)
	, end_(end)
	, thickness_(thickness)
	, color(color) {}

KarkinolutionView::Gui::Geometry::Rectangle KarkinolutionView::Gui::Components::LowLevel::Line::get_bounds() const {
	const float half_thickness = static_cast<float>(thickness_.value()) / 2.0f;
	const float delta_x        = std::abs(end_.x - begin_.x);
	const float delta_y        = std::abs(end_.y - begin_.y);

	return {{(begin_.x + end_.x) / 2.0f, (begin_.y + end_.y) / 2.0f},
			KarkinolutionView::Gui::Geometry::Width{delta_x + 2.0f * half_thickness},
			KarkinolutionView::Gui::Geometry::Height{delta_y + 2.0f * half_thickness}};
}

void KarkinolutionView::Gui::Components::LowLevel::Line::draw() const {
	DrawLineEx(Vector2{.x = begin_.x, .y = begin_.y},
			   Vector2{.x = end_.x, .y = end_.y},
			   static_cast<float>(thickness_.value()),
			   Colors::translate(color));
}