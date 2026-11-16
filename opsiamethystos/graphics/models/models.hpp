#pragma once

#include "opsiamethystos/math/geometry/geometry.hpp"
#include "shared/core/color.hpp"
#include "shared/math/stats/compile_values.hpp"

#include <string>

namespace Opsiamethystos {
	namespace Graphics {
		namespace Models {
			class SimpleText {
				public:

					std::string               text;
					Gui::Geometry::Position   position;
					GreaterThanZeroValue<int> font_size;
					KShared::Colors::Color    color;
			};
		} // namespace Models
	} // namespace Graphics
} // namespace Opsiamethystos