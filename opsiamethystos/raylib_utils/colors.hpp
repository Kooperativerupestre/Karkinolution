#pragma once
#include "shared/core/color.hpp"

#include <raylib.h>

namespace Opsiamethystos {
	namespace Gui {
		using RayLibColor = Color;

		namespace Colors {
			constexpr RayLibColor translate(const KShared::Colors::Color &color) {
				constexpr float scale = 255.0f;

				return {static_cast<unsigned char>(color.r.value.value() * scale),
						static_cast<unsigned char>(color.g.value.value() * scale),
						static_cast<unsigned char>(color.b.value.value() * scale),
						static_cast<unsigned char>(color.a.value.value() * scale)};
			}
		} // namespace Colors
	} // namespace Gui
} // namespace Opsiamethystos
