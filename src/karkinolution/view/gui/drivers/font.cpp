#include "karkinolution/view/gui/drivers/font.hpp"

#include "raylib.h"

#include <algorithm>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace Drivers {
				namespace Font {
					TextMeasurement measure_text(const std::string_view text, const int32_t font_size) {
						constexpr int default_font_size = 10;
						const int draw_font_size = std::max(font_size, default_font_size);
						const int spacing = draw_font_size / default_font_size;
						const auto measured = MeasureTextEx(GetFontDefault(),
															 text.data(),
															 static_cast<float>(draw_font_size),
															 static_cast<float>(spacing));

						return {.width = measured.x, .height = measured.y};
					}
				} // namespace Font
			} // namespace Drivers
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
