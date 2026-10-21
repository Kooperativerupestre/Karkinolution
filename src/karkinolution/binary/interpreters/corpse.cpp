#include "karkinolution/binary/interpreters/corpse.hpp"

#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/interpreters/identity.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/organism/stats.hpp"

#include <cstddef>
#include <karkinolution/binary/interpreters/unit.hpp>
#include <karkinolution/binary/interpreters/vec.hpp>

using OrganismStats::Body::RawMeat;

// SRI implementations
Karkinolution::Binary::Interpreters::CoreBytes::Identity::Id
CorpseSRI::serialize_id(BaseIdType id) {
	return Serializer::convert_uint64_t(id);
}

Serializer::Core::FourBytes CorpseSRI::serialize_raw_meat(const RawMeat &raw_meat) {
	return Serializer::convert_float(raw_meat.value);
}

VecSRI::BytesArrays::Vec CorpseSRI::serialize_position(const Vec3 &position) {
	return VecSRI::serialize_vec(position);
}

UnitsSRI::BytesArrays::Size CorpseSRI::serialize_size(const Size &size) {
	return UnitsSRI::serialize_size(size);
}

CorpseSRI::BytesArrays::Corpse CorpseSRI::serialize_corpse(const class Corpse &corpse) {
	BytesArrays::Corpse bytes{};

	const auto id_bytes = serialize_id(corpse.id);
	Deserializer::append_bytes(bytes, id_bytes, CorpseSRI::Layout::Id::OFFSET);

	const auto raw_meat_bytes = serialize_raw_meat(corpse.raw_meat);
	Deserializer::append_bytes(bytes, raw_meat_bytes, CorpseSRI::Layout::RawMeat::OFFSET);

	const auto position_bytes = serialize_position(corpse.position);
	Deserializer::append_bytes(bytes, position_bytes, CorpseSRI::Layout::PositionX::OFFSET);

	const auto size_bytes = serialize_size(corpse.size);
	Deserializer::append_bytes(bytes, size_bytes, CorpseSRI::Layout::Lateral::OFFSET);

	return bytes;
}

std::array<std::byte, 8> CorpseSRI::Request::make_see_corpse(BaseIdType id) {
	return Serializer::serialize<BaseIdType>(id);
}

// DSI implementations
BaseIdType CorpseDSI::get_id(const std::vector<std::byte> &payload, std::size_t offset) {
	return Deserializer::deserialize<BaseIdType>(payload, CorpseSRI::Layout::Id::OFFSET + offset);
}

RawMeat CorpseDSI::get_raw_meat(const std::vector<std::byte> &payload, std::size_t offset) {
	return RawMeat(Deserializer::read_float(payload, CorpseSRI::Layout::RawMeat::OFFSET + offset));
}

Vec3 CorpseDSI::get_position(const std::vector<std::byte> &payload, std::size_t offset) {
	return VecDSI::get_vec(payload, CorpseSRI::Layout::PositionX::OFFSET + offset);
}

Size CorpseDSI::get_size(const std::vector<std::byte> &payload, std::size_t offset) {
	return UnitsDSI::deserialize_size(payload, CorpseSRI::Layout::Lateral::OFFSET + offset);
}

CorpseDSI::Models::Corpse CorpseDSI::get_corpse(const std::vector<std::byte> &payload,
												std::size_t                   offset) {
	return Models::Corpse{
		.id       = get_id(payload, offset),
		.raw_meat = get_raw_meat(payload, offset),
		.position = get_position(payload, offset),
		.size     = get_size(payload, offset),
	};
}

BaseIdType CorpseDSI::Request::interpret_like_see_corpse(const std::vector<std::byte> &payload) {
	return BaseIdType{Deserializer::deserialize<BaseIdType>(payload, 0)};
}
