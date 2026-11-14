#pragma once

namespace KarkinolutionView {
	namespace Presets {
		struct Grid {
				float cell_size;
				int   columns;
				int   rows;

				float width() const {
					return columns * cell_size;
				}

				float height() const {
					return rows * cell_size;
				}

				float x_offset() const {
					return -(width() / 2.0f);
				}

				float y_offset() const {
					return -(height() / 2.0f);
				}
		};
	} // namespace Presets
} // namespace KarkinolutionView