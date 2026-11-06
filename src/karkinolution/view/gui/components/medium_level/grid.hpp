#pragma once

#include "karkinolution/core/color.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/math/stats/compile_values.hpp"
#include "karkinolution/view/gui/components/base_component.hpp"
#include "karkinolution/view/gui/components/identity.hpp"
#include "karkinolution/view/gui/geometry/geometry.hpp"

#include <cmath>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace MediumLevel {
				class Grid : public BaseComponent<ComponentType::GRID> {
					private:

						const Geometry::Rectangle bounds_;
						const Geometry::Thickness thickness_;
						const Geometry::Width     max_cell_width_;
						const Geometry::Height    max_cell_height_;

						double             get_line_x_based_on_column(Geometry::Column column) const;
						double             get_line_y_based_on_row(Geometry::Row row) const;
						double             get_cell_x_based_on_column(Geometry::Column column) const;
						double             get_cell_y_based_on_row(Geometry::Row row) const;
						Geometry::Position get_cell_position(Geometry::Column column, Geometry::Row row) const;

					public:

						const Karkinolution::Colors::Color color;

						Grid(const Geometry::Rectangle          &bounds,
							 Geometry::Thickness                 thickness,
							 Geometry::Width                     max_cell_width,
							 Geometry::Height                    max_cell_height,
							 const Karkinolution::Colors::Color &color);

						[[nodiscard]]
						Geometry::Rectangle get_bounds() const {
							return bounds_;
						}

						[[nodiscard]]
						Karkinolution::Math::LV::CT::GreaterThanZeroValue<double> size() const {
							return bounds_.width.value() * bounds_.height.value();
						}

						[[nodiscard]]
						NonNegativeValue<int> rows() const {
							return {static_cast<int>(bounds_.height.value() / max_cell_height_.value())};
						}

						[[nodiscard]]
						NonNegativeValue<int> columns() const {
							return {static_cast<int>(bounds_.width.value() / max_cell_width_.value())};
						}

						[[nodiscard]]
						double right() const {
							return bounds_.right().value;
						}

						[[nodiscard]]
						double left() const {
							return bounds_.left().value;
						}

						[[nodiscard]]
						double top() const {
							return bounds_.top().value;
						}

						[[nodiscard]]
						double bottom() const {
							return bounds_.floor().value;
						}

						[[nodiscard]]
						double max_cell_area() const {
							return max_cell_height_.value() * max_cell_width_.value();
						}

						void draw() const;

						void draw_cell(Geometry::Width                     width,
									   Geometry::Height                    height,
									   Geometry::Column                    column,
									   Geometry::Row                       row,
									   const Karkinolution::Colors::Color &cell_color) const;

						void draw_cell(Geometry::Column                    column,
									   Geometry::Row                       row,
									   const Karkinolution::Colors::Color &cell_color) const;
				};
			} // namespace MediumLevel
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView