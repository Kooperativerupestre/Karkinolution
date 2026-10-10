

#include "karkinolution/binary/deserialization/interpreters/territory/territory/territory.hpp"

#include "karkinolution/binary/deserialization/interpreters/math/unit/alias.hpp"
#include "karkinolution/binary/deserialization/interpreters/territory/territory/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/territory/territory/territory.hpp"

#include <karkinolution/binary/serialization/interpreters/territory/territory/alias.hpp>

TerritoryDSI::Models::Territory
TerritoryDSI::deserialize_territory(const std::vector<std::byte> &payload) {
	const auto size = UnitsDSI::deserialize_size(payload, TerritorySRI::Layout::Size::OFFSET);
	return TerritoryDSI::Models::Territory{size};
}
