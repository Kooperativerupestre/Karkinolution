#include "karkinolution/binary/interpreters/territory.hpp"

#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/interpreters/unit.hpp"

// SRI implementations
TerritorySRI::BytesArrays::Territory
TerritorySRI::serialize_territory(const class Territory &territory) {
	TerritorySRI::BytesArrays::Territory bytes{};

	const auto size_bytes = UnitsSRI::serialize_size(territory.size());
	Deserializer::append_bytes(bytes, size_bytes, TerritorySRI::Layout::Size::OFFSET);

	return bytes;
}

// DSI implementations
TerritoryDSI::Models::Territory
TerritoryDSI::deserialize_territory(const std::vector<std::byte> &payload) {
	const auto size = UnitsDSI::deserialize_size(payload, TerritorySRI::Layout::Size::OFFSET);
	return TerritoryDSI::Models::Territory{size};
}
