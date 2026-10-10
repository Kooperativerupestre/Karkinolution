#include "karkinolution/binary/serialization/interpreters/corpse/corpse.hpp"

#include "karkinolution/binary/serialization/interpreters/corpse/alias.hpp"
#include "karkinolution/binary/serialization/interpreters/identity/identity.hpp"
#include "karkinolution/binary/serialization/interpreters/math/unit/unit.hpp"
#include "karkinolution/binary/serialization/interpreters/math/vec/vec.hpp"
#include "karkinolution/core/id.hpp"

#include <cstddef>
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>

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