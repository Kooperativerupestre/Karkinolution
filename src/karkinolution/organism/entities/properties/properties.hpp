#pragma once


#include <cstdint>
#include <unordered_set>
#include <variant>

namespace Properties {
	namespace Capabilities {
		enum class Move : uint8_t {
			WALK,
			SWIMM
		};
	} // namespace Capabilities
} // namespace Properties

using GenericProperty = std::variant<Properties::Capabilities::Move>;

struct PropertyConflict {
		GenericProperty A;
		GenericProperty B;
};

inline constexpr PropertyConflict GLOBAL_PROPERTY_CONFLICTS[] = {};

class PropertiesContainer {
	public:

		std::unordered_set<GenericProperty> value;

		bool has(GenericProperty property) const;
		bool add(GenericProperty property);
		bool remove(GenericProperty property);
		void clear();
};
