#include "karkinolution/binary/interpreters/corpse.hpp"

#include "karkinolution/binary/byte_utils.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/interpreters/identity.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/core/id.hpp"

#include <cstddef>
#include <karkinolution/binary/interpreters/unit.hpp>
#include <karkinolution/binary/interpreters/vec.hpp>

// SRI implementations
Karkinolution::Binary::Interpreters::CoreBytes::Identity::Id
CorpseSRI::serialize_id(Karkinolution::Core::Id::BaseIdType id) {
	return Karkinolution::Binary::Serializer::convert_uint64_t(id);
}

Karkinolution::Binary::Serializer::Core::FourBytes
CorpseSRI::serialize_raw_meat(const Karkinolution::Organisms::RawMeat &raw_meat) {
	return Karkinolution::Binary::Serializer::convert_float(raw_meat.value);
}

VecSRI::BytesArrays::Vec
CorpseSRI::serialize_position(const Karkinolution::Math::Vec::Vec2 &position) {
	return VecSRI::serialize_vec(position);
}

UnitsSRI::BytesArrays::Size
CorpseSRI::serialize_size(const Karkinolution::Math::Units::Size &size) {
	return UnitsSRI::serialize_size(size);
}

CorpseSRI::BytesArrays::Corpse
CorpseSRI::serialize_corpse(const Karkinolution::Organisms::Entities::Corpse::Corpse &corpse) {
	BytesArrays::Corpse bytes{};

	const auto id_bytes = serialize_id(corpse.id);
	Karkinolution::Binary::Utils::append_bytes(bytes, id_bytes, CorpseSRI::Layout::Id::OFFSET);

	const auto raw_meat_bytes = serialize_raw_meat(corpse.raw_meat);
	Karkinolution::Binary::Utils::append_bytes(bytes,
											   raw_meat_bytes,
											   CorpseSRI::Layout::RawMeat::OFFSET);

	const auto position_bytes = serialize_position(corpse.position);
	Karkinolution::Binary::Utils::append_bytes(bytes,
											   position_bytes,
											   CorpseSRI::Layout::PositionX::OFFSET);

	const auto size_bytes = serialize_size(corpse.size);
	Karkinolution::Binary::Utils::append_bytes(bytes, size_bytes, CorpseSRI::Layout::Width::OFFSET);

	return bytes;
}

std::array<std::byte, 8>
CorpseSRI::Request::make_see_corpse(Karkinolution::Core::Id::BaseIdType id) {
	return Karkinolution::Binary::Serializer::serialize<Karkinolution::Core::Id::BaseIdType>(id);
}

// DSI implementations
Karkinolution::Core::Id::BaseIdType CorpseDSI::get_id(const std::vector<std::byte> &payload,
													  std::size_t                   offset) {
	return Karkinolution::Binary::Deserializer::deserialize<Karkinolution::Core::Id::BaseIdType>(
		payload,
		offset);
}

Karkinolution::Organisms::RawMeat CorpseDSI::get_raw_meat(const std::vector<std::byte> &payload,
														  std::size_t                   offset) {
	return Karkinolution::Organisms::RawMeat(
		Karkinolution::Binary::Deserializer::read_float(payload, offset));
}

Karkinolution::Math::Vec::Vec2 CorpseDSI::get_position(const std::vector<std::byte> &payload,
													   std::size_t                   offset) {
	return VecDSI::get_vec(payload, offset);
}

Karkinolution::Math::Units::Size CorpseDSI::get_size(const std::vector<std::byte> &payload,
													 std::size_t                   offset) {
	return UnitsDSI::deserialize_size(payload, offset);
}

Karkinolution::Binary::Interpreters::Models::Corpse::Corpse
CorpseDSI::get_corpse(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Binary::Interpreters::Models::Corpse::Corpse{
		.id       = get_id(payload, offset + CorpseSRI::Layout::Id::OFFSET),
		.raw_meat = get_raw_meat(payload, offset + CorpseSRI::Layout::RawMeat::OFFSET),
		.position = get_position(payload, offset + CorpseSRI::Layout::PositionX::OFFSET),
		.size     = get_size(payload, offset + CorpseSRI::Layout::Width::OFFSET),
	};
}

Karkinolution::Core::Id::BaseIdType
CorpseDSI::Request::interpret_like_see_corpse(const std::vector<std::byte> &payload) {
	return Karkinolution::Core::Id::BaseIdType{
		Karkinolution::Binary::Deserializer::deserialize<Karkinolution::Core::Id::BaseIdType>(
			payload,
			0)};
}
