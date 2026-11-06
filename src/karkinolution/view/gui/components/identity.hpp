#pragma once
#include "karkinolution/core/id.hpp"

#include <cstdint>
#include <functional>

namespace KarkinolutionView {
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
					Karkinolution::Core::Id::BaseIdType id;
					ComponentType                       type;
					bool                                operator==(const ComponentId &) const noexcept = default;
			};

			inline ComponentId gen_id(ComponentType type) {
				return {Karkinolution::Core::Id::gen_id(), type};
			}
		} // namespace Components
	} // namespace Gui
} // namespace KarkinolutionView

namespace std {
	template <> struct hash<KarkinolutionView::Gui::Components::ComponentId> {
			[[nodiscard]] std::size_t
			operator()(const KarkinolutionView::Gui::Components::ComponentId &component) const noexcept {
				const auto id_hash = std::hash<Karkinolution::Core::Id::BaseIdType>{}(component.id);

				const auto type_hash = std::hash<KarkinolutionView::Gui::Components::ComponentType>{}(component.type);

				return id_hash ^ (type_hash + 0x9e3779b9 + (id_hash << 6) + (id_hash >> 2));
			}
	};
} // namespace std