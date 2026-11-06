#pragma once

#include <cstdint>
#include <string_view>

namespace KarkinolutionView {
	namespace Gui {
		namespace Components {
			namespace Drivers {
				namespace Font {
					struct TextMeasurement {
						float width;
						float height;
					};

					[[nodiscard]] TextMeasurement measure_text(std::string_view text, int32_t font_size);
				} // namespace Font
			} // namespace Drivers
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView
