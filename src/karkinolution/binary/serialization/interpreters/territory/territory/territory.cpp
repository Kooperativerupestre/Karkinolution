#include "karkinolution/binary/serialization/interpreters/territory/territory/territory.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/serialization/interpreters/territory/territory/alias.hpp>

TerritorySRI::BytesArrays::Territory
TerritorySRI::serialize_territory(const class Territory &territory) {
	TerritorySRI::BytesArrays::Territory bytes{};

	const auto size_bytes = UnitsSRI::serialize_size(territory.size());
	Deserializer::append_bytes(bytes, size_bytes, TerritorySRI::Layout::Size::OFFSET);

	return bytes;
}
