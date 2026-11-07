#pragma once

#include <cstdint>
#include <string_view>

namespace Opsiamethystos {
	namespace Gui {
		namespace Components {
			namespace Drivers {
				namespace Font {
					struct TextMeasurement {
							// Width and height are the rendered extents in GUI units. Keeping this
							// value object independent of Raylib lets Text use the same contract in
							// production and deterministic layout tests.
							float width;
							float height;
					};

					// Measures with the same default-font size clamp and spacing that the
					// rendering call uses. The Raylib default font must already be initialized
					// by the window layer before production code calls this function.
					[[nodiscard]] TextMeasurement measure_text(std::string_view text, int32_t font_size);
				} // namespace Font
			} // namespace Drivers
		} // namespace Components
	} // namespace Gui
} // namespace Opsiamethystos
