#pragma once

#include "opsiamethystos/graphics/panel/components/identity.hpp"

#include <cstdint>
#include <string_view>

namespace Opsiamethystos {
	namespace Gui {
		namespace Panel {
			// Reasons a layout pass cannot be committed. Each code is a recoverable runtime
			// condition. Invalid layout declarations are programming errors and throw instead.
			enum class LayoutErrorCode : uint8_t {
				// A layout leaf references a component that is not in the panel, for example
				// because it was removed after set_layout.
				UNKNOWN_COMPONENT,
				// A computed slot is not contained by the panel boundary.
				SLOT_OUTSIDE_PANEL,
				// The component refused its slot, for example a Line, which has no rectangular slot.
				COMPONENT_REJECTED_SLOT,
				// The spatial index refused the new slot.
				INDEX_REJECTED_SLOT
			};

			// Identifies the first component whose slot could not be applied. The panel is left
			// exactly as it was before the failed pass.
			struct LayoutError {
					LayoutErrorCode         code;
					Components::ComponentId component;
			};

			// Human-readable name of a code, intended for logs and assertion messages.
			[[nodiscard]] constexpr std::string_view to_string(const LayoutErrorCode code) noexcept {
				switch (code) {
					case LayoutErrorCode::UNKNOWN_COMPONENT:
						return "layout references a component that is not in the panel";
					case LayoutErrorCode::SLOT_OUTSIDE_PANEL:
						return "computed slot is outside the panel boundary";
					case LayoutErrorCode::COMPONENT_REJECTED_SLOT:
						return "component rejected its slot";
					case LayoutErrorCode::INDEX_REJECTED_SLOT:
						return "spatial index rejected the slot";
				}
				return "unknown layout error";
			}
		} // namespace Panel
	} // namespace Gui
} // namespace Opsiamethystos
