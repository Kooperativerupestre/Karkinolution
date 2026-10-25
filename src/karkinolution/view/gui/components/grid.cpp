#include "karkinolution/core/color.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/view/gui/components/line.hpp"
#include "karkinolution/view/gui/components/rectangle.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <format>
#include <karkinolution/view/gui/colors.hpp>
#include <karkinolution/view/gui/components/grid.hpp>

void KarkinolutionView::Gui::Components::Grid::draw() const {
	for (size_t y = 0; y <= columns().value(); y++) {
		const float const_x = get_line_x_based_on_column(Geometry::Column{y});

		Components::Line column_line{
			.begin     = Geometry::Position{const_x, static_cast<float>(top())},
			.end       = Geometry::Position{const_x, static_cast<float>(bottom())},
			.thickness = thickness,
			.color     = color};

		column_line.draw();
	}

	for (size_t x = 0; x <= rows().value(); x++) {
		const float const_y = get_line_y_based_on_row(Geometry::Row{x});

		Components::Line row_line{.begin = Geometry::Position{static_cast<float>(left()), const_y},
								  .end   = Geometry::Position{static_cast<float>(right()), const_y},
								  .thickness = thickness,
								  .color     = color};

		row_line.draw();
	}
}

void KarkinolutionView::Gui::Components::Grid::draw_cell(
	Geometry::Column                    column,
	Geometry::Row                       row,
	const Karkinolution::Colors::Color &cell_color) const {
	return draw_cell(max_cell_width, max_cell_height, column, row, cell_color);
}

void KarkinolutionView::Gui::Components::Grid::draw_cell(
	Geometry::Width                     width,
	Geometry::Height                    height,
	Geometry::Column                    column,
	Geometry::Row                       row,
	const Karkinolution::Colors::Color &cell_color) const {
	if (width > max_cell_width) {
		throw SimulationError(std::format("Width {} of cell of grid to draw is larger than max {}",
										  width.value(),
										  max_cell_width.value()));
	}

	if (height > max_cell_height) {
		throw SimulationError(std::format("Height {} of cell of grid to draw is larger than max{}",
										  height.value(),
										  max_cell_height.value()));
	}

	const float                      half = static_cast<float>(thickness.value() / 2.0);
	const auto                       pos  = get_cell_position(column, row);
	const Gui::Components::Rectangle cell_rectangle{
		.value = Gui::Geometry::Rectangle{Geometry::Position{pos.x + half, pos.y + half},
										  Geometry::Width{width - thickness.value()},
										  Geometry::Height{height - thickness.value()}},
		.color = cell_color};
	cell_rectangle.draw();
}