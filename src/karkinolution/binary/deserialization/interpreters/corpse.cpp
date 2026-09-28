#include "karkinolution/binary/deserialization/interpreters/corpse.hpp"

#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp"
#include "karkinolution/binary/deserialization/interpreters/math/vec.hpp"
#include "karkinolution/binary/serialization/interpreters/corpse.hpp"

std::uint64_t CorpseResponseDSI::get_id(const std::vector<std::byte> &payload,
										std::size_t                   offset) {
	return Deserializer::read_uint64_t(payload, offset + CorpseSRI::TO_GET_ID_OFFSET);
}

RawMeat CorpseResponseDSI::get_raw_meat(const std::vector<std::byte> &payload,
										std::size_t                   offset) {
	return RawMeat(Deserializer::read_float(payload, offset + CorpseSRI::TO_GET_RAW_MEAT_OFFSET));
}

Vec3 CorpseResponseDSI::get_position(const std::vector<std::byte> &payload, std::size_t offset) {
	return VecDSI::deserialize_vec(payload, offset + CorpseSRI::TO_GET_POSITION_OFFSET);
}

Size CorpseResponseDSI::get_size(const std::vector<std::byte> &payload, std::size_t offset) {
	return PhysicsUnitsDSI::deserialize_size(payload, offset + CorpseSRI::TO_GET_SIZE_OFFSET);
}

DesserializedCorpse CorpseResponseDSI::get_corpse(const std::vector<std::byte> &payload,
												 std::size_t                   offset) {
	return DesserializedCorpse{
		.id       = get_id(payload, offset),
		.raw_meat = get_raw_meat(payload, offset),
		.position = get_position(payload, offset),
		.size     = get_size(payload, offset),
	};
}

BaseIdType CorpseRequestDSI::interpret_like_get_corpse(const std::vector<std::byte> &payload,
													   std::size_t                   offset) {
	return BaseIdType{Deserializer::read_uint64_t(payload, offset)};
}
