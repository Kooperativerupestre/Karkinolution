#include "opsiamethystos/drivers/font.hpp"

#include "raylib.h"

#include <algorithm>

namespace Opsiamethystos {
	namespace Gui {
		namespace Components {
			namespace Drivers {
				namespace Font {
					// DrawText derives spacing from the effective font size in this project.
					// Measuring with the same rule prevents Content layout from producing a
					// slot that differs from the pixels ultimately drawn.
					TextMeasurement measure_text(const std::string_view text, const int32_t font_size) {
						constexpr int default_font_size = 10;
						const int     draw_font_size    = std::max(font_size, default_font_size);
						const int     spacing           = draw_font_size / default_font_size;
						const auto    measured          = MeasureTextEx(GetFontDefault(),
                                                            text.data(),
                                                            static_cast<float>(draw_font_size),
                                                            static_cast<float>(spacing));

						return {.width = measured.x, .height = measured.y};
					}
				} // namespace Font
			} // namespace Drivers
		} // namespace Components
	} // namespace Gui
} // namespace Opsiamethystos
