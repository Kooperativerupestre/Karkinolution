#include "karkinolution/binary/serialization/interpreters/corpse.hpp"

#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>

Serializer::Types::Uint64tBytes CorpseSRI::serialize_id(std::uint64_t id) {
	return Serializer::convert_uint64_t(id);
}

Serializer::Types::FloatBytes CorpseSRI::serialize_raw_meat(const RawMeat &raw_meat) {
	return Serializer::convert_float(raw_meat.value);
}

VecSRI::VecBytes CorpseSRI::serialize_position(const Vec3 &position) {
	return VecSRI::serialize_vec(position);
}

PhysicsUnitsSRI::SizeBytes CorpseSRI::serialize_size(const Size &size) {
	return PhysicsUnitsSRI::serialize_size(size);
}

CorpseSRI::CorpseBytes CorpseSRI::serialize_corpse(const Corpse &corpse) {
	CorpseBytes bytes{};

	const auto id_bytes = serialize_id(corpse.id);
	Deserializer::append_bytes(bytes, id_bytes, TO_GET_ID_OFFSET);

	const auto raw_meat_bytes = serialize_raw_meat(corpse.raw_meat);
	Deserializer::append_bytes(bytes, raw_meat_bytes, TO_GET_RAW_MEAT_OFFSET);

	const auto position_bytes = serialize_position(corpse.position);
	Deserializer::append_bytes(bytes, position_bytes, TO_GET_POSITION_OFFSET);

	const auto size_bytes = serialize_size(corpse.size);
	Deserializer::append_bytes(bytes, size_bytes, TO_GET_SIZE_OFFSET);

	return bytes;
}
