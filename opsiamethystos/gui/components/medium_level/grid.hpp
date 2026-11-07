#pragma once

#include "opsiamethystos/gui/components/base_component.hpp"
#include "opsiamethystos/gui/components/identity.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"
#include "shared/core/error.hpp"
#include "shared/math/stats/compile_values.hpp"

#include <cmath>

namespace Opsiamethystos {
	namespace Gui {
		namespace Components {
			namespace MediumLevel {
				class Grid : public BaseComponent<ComponentType::GRID> {
					private:

						Geometry::Rectangle       bounds_;
						const Geometry::Thickness thickness_;
						const Geometry::Width     max_cell_width_;
						const Geometry::Height    max_cell_height_;

						double             get_line_x_based_on_column(Geometry::Column column) const;
						double             get_line_y_based_on_row(Geometry::Row row) const;
						double             get_cell_x_based_on_column(Geometry::Column column) const;
						double             get_cell_y_based_on_row(Geometry::Row row) const;
						Geometry::Position get_cell_position(Geometry::Column column, Geometry::Row row) const;

					public:

						const KShared::Colors::Color color;

						Grid(const Geometry::Rectangle    &bounds,
							 Geometry::Thickness           thickness,
							 Geometry::Width               max_cell_width,
							 Geometry::Height              max_cell_height,
							 const KShared::Colors::Color &color);

						[[nodiscard]]
						Geometry::Rectangle get_bounds() const {
							return bounds_;
						}

						// Cell limits remain unchanged; only the drawable grid rectangle follows
						// the layout slot, so rows and columns are recomputed from its size.
						bool set_slot(const Geometry::Rectangle &slot) {
							bounds_ = slot;
							return true;
						}

						[[nodiscard]]
						KShared::Math::LV::CT::GreaterThanZeroValue<double> size() const {
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
						// Grid geometry remains panel-local; the origin is applied only when
						// constructing the rendered lines and cells.
						void draw(const Geometry::Position &origin) const;

						// The explicit-size overload validates cell dimensions before applying
						// the same origin used by the grid lines.
						void draw_cell(Geometry::Width               width,
									   Geometry::Height              height,
									   Geometry::Column              column,
									   Geometry::Row                 row,
									   const KShared::Colors::Color &cell_color) const;

						void draw_cell(Geometry::Width               width,
									   Geometry::Height              height,
									   Geometry::Column              column,
									   Geometry::Row                 row,
									   const KShared::Colors::Color &cell_color,
									   const Geometry::Position     &origin) const;

						void draw_cell(Geometry::Column              column,
									   Geometry::Row                 row,
									   const KShared::Colors::Color &cell_color) const;

						void draw_cell(Geometry::Column              column,
									   Geometry::Row                 row,
									   const KShared::Colors::Color &cell_color,
									   const Geometry::Position     &origin) const;
				};
			} // namespace MediumLevel
		} // namespace Components
	} // namespace Gui
} // namespace Opsiamethystos
