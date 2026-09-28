#include "karkinolution/binary/deserialization/interpreters/territory/territory.hpp"

#include "karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp"
#include "karkinolution/binary/serialization/interpreters/territory/territory.hpp"

namespace TerritoryDSI {

Territory deserialize_territory(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto size =
		PhysicsUnitsDSI::deserialize_size(payload, offset + TerritorySRI::TO_GET_SIZE_OFFSET);
	return Territory{size};
}

} // namespace TerritoryDSI
