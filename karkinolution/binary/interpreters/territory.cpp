#include "karkinolution/binary/interpreters/territory.hpp"

#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/interpreters/unit.hpp"
#include "shared/binary/serializer.hpp"

TerritorySRI::BytesArrays::Territory
TerritorySRI::serialize_territory(const Karkinolution::Binary::Interpreters::Models::Territory::Territory &territory) {
	TerritorySRI::BytesArrays::Territory bytes{};

	const auto size_bytes = UnitsSRI::serialize_size(territory.size);
	KShared::Binary::Utils::append_bytes(bytes, size_bytes, TerritorySRI::Layout::Size::OFFSET);

	return bytes;
}

std::array<std::byte, 0> TerritorySRI::Request::make_see_territory() {
	return {};
}

Karkinolution::Binary::Interpreters::Models::Territory::Territory
TerritoryDSI::deserialize_territory(const std::vector<std::byte> &payload, std::size_t offset) {
	const auto size = UnitsDSI::deserialize_size(payload, offset + TerritorySRI::Layout::Size::OFFSET);
	return Karkinolution::Binary::Interpreters::Models::Territory::Territory{size};
}
