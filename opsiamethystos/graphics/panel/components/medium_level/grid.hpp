#pragma once

#include "opsiamethystos/gui/components/base_component.hpp"
#include "opsiamethystos/gui/components/identity.hpp"
#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"
#include "shared/math/stats/compile_values.hpp"

namespace Opsiamethystos {
	namespace Graphics {
		namespace Panel {
			namespace Components {
				namespace MediumLevel {
					class Grid : public Gui::Components::BaseComponent<Gui::Components::ComponentType::GRID> {
						private:

							Math::Geometry::Rectangle       bounds_;
							const Math::Geometry::Thickness thickness_;
							const Math::Geometry::Width     max_cell_width_;
							const Math::Geometry::Height    max_cell_height_;

							double                   get_line_x_based_on_column(Math::Geometry::Column column) const;
							double                   get_line_y_based_on_row(Math::Geometry::Row row) const;
							double                   get_cell_x_based_on_column(Math::Geometry::Column column) const;
							double                   get_cell_y_based_on_row(Math::Geometry::Row row) const;
							Math::Geometry::Position get_cell_position(Math::Geometry::Column column,
																	   Math::Geometry::Row    row) const;

						public:

							const KShared::Colors::Color color;

							Grid(const Math::Geometry::Rectangle &bounds,
								 Math::Geometry::Thickness        thickness,
								 Math::Geometry::Width            max_cell_width,
								 Math::Geometry::Height           max_cell_height,
								 const KShared::Colors::Color    &color);

							[[nodiscard]]
							Math::Geometry::Rectangle get_bounds() const {
								return bounds_;
							}

							// Cell limits remain unchanged; only the drawable grid rectangle follows
							// the layout slot, so rows and columns are recomputed from its size.
							bool set_slot(const Math::Geometry::Rectangle &slot) {
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
							void draw(const Math::Geometry::Position &origin) const;

							// The explicit-size overload validates cell dimensions before applying
							// the same origin used by the grid lines.
							void draw_cell(Math::Geometry::Width         width,
										   Math::Geometry::Height        height,
										   Math::Geometry::Column        column,
										   Math::Geometry::Row           row,
										   const KShared::Colors::Color &cell_color) const;

							void draw_cell(Math::Geometry::Width           width,
										   Math::Geometry::Height          height,
										   Math::Geometry::Column          column,
										   Math::Geometry::Row             row,
										   const KShared::Colors::Color   &cell_color,
										   const Math::Geometry::Position &origin) const;

							void draw_cell(Math::Geometry::Column        column,
										   Math::Geometry::Row           row,
										   const KShared::Colors::Color &cell_color) const;

							void draw_cell(Math::Geometry::Column          column,
										   Math::Geometry::Row             row,
										   const KShared::Colors::Color   &cell_color,
										   const Math::Geometry::Position &origin) const;
					};
				} // namespace MediumLevel
			} // namespace Components
		} // namespace Panel
	} // namespace Graphics
} // namespace Opsiamethystos
