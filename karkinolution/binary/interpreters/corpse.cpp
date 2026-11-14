#include "karkinolution/binary/interpreters/corpse.hpp"

#include "karkinolution/binary/interpreters/identity.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/serializer.hpp"
#include "shared/core/id.hpp"

#include <cstddef>
#include <shared/binary/interpreters/unit.hpp>
#include <shared/binary/interpreters/vec.hpp>

// SRI implementations
Karkinolution::Binary::Interpreters::CoreBytes::Identity::Id CorpseSRI::serialize_id(KShared::Core::Id::BaseIdType id) {
	return KShared::Binary::Serializer::convert_uint64_t(id);
}

KShared::Binary::Serializer::Core::FourBytes
CorpseSRI::serialize_raw_meat(const Karkinolution::Organisms::RawMeat &raw_meat) {
	return KShared::Binary::Serializer::convert_float(raw_meat.value);
}

VecSRI::BytesArrays::Vec CorpseSRI::serialize_position(const KShared::Math::Vec::Vec2 &position) {
	return VecSRI::serialize_vec(position);
}

UnitsSRI::BytesArrays::Size CorpseSRI::serialize_size(const KShared::Math::Units::Size &size) {
	return UnitsSRI::serialize_size(size);
}

CorpseSRI::BytesArrays::Corpse
CorpseSRI::serialize_corpse(const Karkinolution::Organisms::Entities::Corpse::Corpse &corpse) {
	BytesArrays::Corpse bytes{};

	const auto id_bytes = serialize_id(corpse.id);
	KShared::Binary::Utils::append_bytes(bytes, id_bytes, CorpseSRI::Layout::Id::OFFSET);

	const auto raw_meat_bytes = serialize_raw_meat(corpse.raw_meat);
	KShared::Binary::Utils::append_bytes(bytes, raw_meat_bytes, CorpseSRI::Layout::RawMeat::OFFSET);

	const auto position_bytes = serialize_position(corpse.position);
	KShared::Binary::Utils::append_bytes(bytes, position_bytes, CorpseSRI::Layout::PositionX::OFFSET);

	const auto size_bytes = serialize_size(corpse.size);
	KShared::Binary::Utils::append_bytes(bytes, size_bytes, CorpseSRI::Layout::Width::OFFSET);

	return bytes;
}

std::array<std::byte, 8> CorpseSRI::Request::make_see_corpse(KShared::Core::Id::BaseIdType id) {
	return KShared::Binary::Serializer::serialize<KShared::Core::Id::BaseIdType>(id);
}

// DSI implementations
KShared::Core::Id::BaseIdType CorpseDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
	return KShared::Binary::Deserializer::deserialize<KShared::Core::Id::BaseIdType>(payload, offset);
}

Karkinolution::Organisms::RawMeat CorpseDSI::get_raw_meat(const std::vector<std::byte> &payload, std::size_t offset) {
	return Karkinolution::Organisms::RawMeat(KShared::Binary::Deserializer::read_float(payload, offset));
}

KShared::Math::Vec::Vec2 CorpseDSI::get_position(const std::vector<std::byte> &payload, std::size_t offset) {
	return VecDSI::get_vec(payload, offset);
}

KShared::Math::Units::Size CorpseDSI::get_size(const std::vector<std::byte> &payload, std::size_t offset) {
	return UnitsDSI::deserialize_size(payload, offset);
}

Karkinolution::Binary::Interpreters::Models::Corpse::Corpse CorpseDSI::get_corpse(const std::vector<std::byte> &payload,
																				  std::size_t offset) {
	return Karkinolution::Binary::Interpreters::Models::Corpse::Corpse{
		.id       = get_id(payload, offset + CorpseSRI::Layout::Id::OFFSET),
		.raw_meat = get_raw_meat(payload, offset + CorpseSRI::Layout::RawMeat::OFFSET),
		.position = get_position(payload, offset + CorpseSRI::Layout::PositionX::OFFSET),
		.size     = get_size(payload, offset + CorpseSRI::Layout::Width::OFFSET),
	};
}

KShared::Core::Id::BaseIdType CorpseDSI::Request::interpret_like_see_corpse(const std::vector<std::byte> &payload) {
	return KShared::Core::Id::BaseIdType{
		KShared::Binary::Deserializer::deserialize<KShared::Core::Id::BaseIdType>(payload, 0)};
}
