

#include "karkinolution/binary/serialization/interpreters/corpse/corpse.hpp"

#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/deserialization/interpreters/corpse/alias.hpp"
#include "karkinolution/binary/deserialization/interpreters/corpse/corpse.hpp"
#include "karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp"
#include "karkinolution/binary/deserialization/interpreters/math/vec/alias.hpp"
#include "karkinolution/organism/stats.hpp"

#include <karkinolution/binary/deserialization/interpreters/math/unit/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/corpse/alias.hpp>

using OrganismStats::Body::RawMeat;

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
