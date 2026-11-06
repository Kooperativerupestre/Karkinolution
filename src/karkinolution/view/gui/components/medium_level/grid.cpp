#include "karkinolution/view/gui/components/medium_level/grid.hpp"

#include "karkinolution/core/error.hpp"
#include "karkinolution/view/gui/components/low_level/line.hpp"
#include "karkinolution/view/gui/components/low_level/rectangle.hpp"

#include <format>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace MediumLevel {
				Grid::Grid(const Geometry::Rectangle          &bounds,
						   Geometry::Thickness                 thickness,
						   Geometry::Width                     max_cell_width,
						   Geometry::Height                    max_cell_height,
						   const Karkinolution::Colors::Color &color)
					: bounds_(bounds)
					, thickness_(thickness)
					, max_cell_width_(max_cell_width)
					, max_cell_height_(max_cell_height)
					, color(color) {}

				double Grid::get_line_x_based_on_column(Geometry::Column column) const {
					if (column.value() > columns().value()) {
						throw Karkinolution::Errors::SimulationError("Column is outside of grid bounds");
					}
					return left() + column.value() * max_cell_width_.value();
				}

				double Grid::get_line_y_based_on_row(Geometry::Row row) const {
					if (row.value() > rows().value()) {
						throw Karkinolution::Errors::SimulationError("Row is outside of grid bounds");
					}
					return top() + row.value() * max_cell_height_.value();
				}

				double Grid::get_cell_x_based_on_column(Geometry::Column column) const {
					if (column.value() >= columns().value()) {
						throw Karkinolution::Errors::SimulationError("Column is outside of cell bounds of the grid");
					}
					return left() + column.value() * max_cell_width_.value();
				}

				double Grid::get_cell_y_based_on_row(Geometry::Row row) const {
					if (row.value() >= rows().value()) {
						throw Karkinolution::Errors::SimulationError("Row is outside of cell bounds of the grid");
					}
					return top() + row.value() * max_cell_height_.value();
				}

				Geometry::Position Grid::get_cell_position(Geometry::Column column, Geometry::Row row) const {
					return {static_cast<float>(get_cell_x_based_on_column(column)),
							static_cast<float>(get_cell_y_based_on_row(row))};
				}

				void Grid::draw() const {
					for (int column = 0; column <= columns().value(); ++column) {
						const float    x = static_cast<float>(get_line_x_based_on_column(Geometry::Column{column}));
						LowLevel::Line line{{x, static_cast<float>(top())},
											{x, static_cast<float>(bottom())},
											thickness_,
											color};
						line.draw();
					}

					for (int row = 0; row <= rows().value(); ++row) {
						const float    y = static_cast<float>(get_line_y_based_on_row(Geometry::Row{row}));
						LowLevel::Line line{{static_cast<float>(left()), y},
											{static_cast<float>(right()), y},
											thickness_,
											color};
						line.draw();
					}
				}

				void Grid::draw_cell(Geometry::Column                    column,
									 Geometry::Row                       row,
									 const Karkinolution::Colors::Color &cell_color) const {
					draw_cell(max_cell_width_, max_cell_height_, column, row, cell_color);
				}

				void Grid::draw_cell(Geometry::Width                     width,
									 Geometry::Height                    height,
									 Geometry::Column                    column,
									 Geometry::Row                       row,
									 const Karkinolution::Colors::Color &cell_color) const {
					if (width > max_cell_width_) {
						throw Karkinolution::Errors::SimulationError(
							std::format("Width {} of cell of grid to draw is larger than max {}",
										width.value(),
										max_cell_width_.value()));
					}
					if (height > max_cell_height_) {
						throw Karkinolution::Errors::SimulationError(
							std::format("Height {} of cell of grid to draw is larger than max {}",
										height.value(),
										max_cell_height_.value()));
					}

					const auto                position = get_cell_position(column, row);
					const LowLevel::Rectangle cell_rectangle{{{position.x + static_cast<float>(width.value()) / 2.0f,
															   position.y + static_cast<float>(height.value()) / 2.0f},
															  Geometry::Width{width.value() - thickness_.value()},
															  Geometry::Height{height.value() - thickness_.value()}},
															 cell_color};
					cell_rectangle.draw();
				}
			} // namespace MediumLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
