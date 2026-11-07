#pragma once
#include "shared/core/id.hpp"

#include <cstdint>
#include <functional>

namespace Opsiamethystos {
	namespace Gui {
		namespace Components {
			enum class ComponentType : uint8_t {
				BUTTON,
				GRID,
				CIRCLE,
				LINE,
				RECTANGLE,
				TEXT,
				RECTANGLE_LABEL,
				CIRCLE_LABEL
			};

			struct ComponentId {
					KShared::Core::Id::BaseIdType id;
					ComponentType                 type;
					bool                          operator==(const ComponentId &) const noexcept = default;
			};

			inline ComponentId gen_id(ComponentType type) {
				return {KShared::Core::Id::gen_id(), type};
			}
		} // namespace Components
	} // namespace Gui
} // namespace Opsiamethystos

namespace std {
	template <> struct hash<Opsiamethystos::Gui::Components::ComponentId> {
			[[nodiscard]] std::size_t
			operator()(const Opsiamethystos::Gui::Components::ComponentId &component) const noexcept {
				const auto id_hash = std::hash<KShared::Core::Id::BaseIdType>{}(component.id);

				const auto type_hash = std::hash<Opsiamethystos::Gui::Components::ComponentType>{}(component.type);

				return id_hash ^ (type_hash + 0x9e3779b9 + (id_hash << 6) + (id_hash >> 2));
			}
	};
} // namespace std