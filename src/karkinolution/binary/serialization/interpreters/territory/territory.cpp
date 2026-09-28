#include "karkinolution/binary/serialization/interpreters/territory/territory.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>

namespace TerritorySRI {

TerritoryBytes serialize_territory(const Territory &territory) {
	TerritoryBytes bytes{};

	const auto size_bytes = PhysicsUnitsSRI::serialize_size(territory.size());
	Deserializer::append_bytes(bytes, size_bytes, TO_GET_SIZE_OFFSET);

	return bytes;
}

} // namespace TerritorySRI
