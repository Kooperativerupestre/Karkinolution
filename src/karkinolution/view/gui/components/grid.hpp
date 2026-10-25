#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <cmath>

namespace KarkinolutionView {

	namespace Gui {
		namespace Components {
			class Grid {
				public:


					Karkinolution::Colors::Color               color;
					KarkinolutionView::Gui::Geometry::Position position;

					Geometry::Thickness thickness;
					Geometry::Width     width;
					Geometry::Height    height;
					Geometry::Width     max_cell_width;
					Geometry::Height    max_cell_height;

					Karkinolution::Math::LV::CompileTime::GreaterThanZeroValue<
						double> constexpr size() const {
						return width.value() * height.value();
					}

					NonNegativeValue<int> constexpr rows() const {
						return {static_cast<int>(height / max_cell_height)};
					}

					NonNegativeValue<int> constexpr columns() const {
						return {static_cast<int>(width / max_cell_width)};
					}

					double constexpr right() const {
						return position.x + width.value() / 2.0;
					}

					double constexpr left() const {
						return position.x - width.value() / 2.0;
					}

					double constexpr top() const {
						return position.y - height.value() / 2.0;
					}

					double constexpr bottom() const {
						return position.y + height.value() / 2.0;
					}

					double constexpr max_cell_area() const {
						return max_cell_height.value() * max_cell_width.value();
					}

					void draw() const;

					void draw_cell(Geometry::Width                     width,
								   Geometry::Height                    height,
								   Geometry::Column                    column,
								   Geometry::Row                       row,
								   const Karkinolution::Colors::Color &cell_color) const;

					void draw_cell(Geometry::Column column,
								   Geometry::Row,
								   const Karkinolution::Colors::Color &cell_color) const;

				private:


					double constexpr get_line_x_based_on_column(Geometry::Column column) const {
						if (column.value() > columns().value()) {
							throw SimulationError("Column is outside of grid bounds");
						}
						return left() + column.value() * max_cell_width.value();
					}

					double constexpr get_line_y_based_on_row(Geometry::Row row) const {
						if (row.value() > rows().value()) {
							throw SimulationError("Row is outside of grid bounds");
						}
						return top() + row.value() * max_cell_height.value();
					}

					// begin = 0
					// end = rows() + 1


					double constexpr get_cell_x_based_on_column(Geometry::Column column) const {
						if (column.value() >= columns().value()) {
							throw SimulationError("Column is outside of cell bounds of the grid");
						}
						return left() + column.value() * max_cell_width.value();
					}

					// begin = 0;
					// end = columns() + 1

					double constexpr get_cell_y_based_on_row(Geometry::Row row) const {
						if (row >= rows().value()) {
							throw SimulationError("Row is outside of cell bounds of the grid");
						}
						return top() + row.value() * max_cell_height.value();
					}

					Geometry::Position get_cell_position(Geometry::Column column,
														 Geometry::Row    row) const {
						return {static_cast<float>(get_cell_x_based_on_column(column)),
								static_cast<float>(get_cell_y_based_on_row(row))};
					}
			};

		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView