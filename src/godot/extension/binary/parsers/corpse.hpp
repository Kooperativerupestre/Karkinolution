#pragma once

#include "binary/parsers/byte_utils.hpp"
#include "model/corpse/corpse.hpp"

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <karkinolution/binary/deserialization/interpreters/corpse.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>

namespace GodotBinaryParser {

	inline godot::Ref<GodotCorpse> parse_corpse(const godot::PackedByteArray &payload,
	                                             std::uint64_t                 id = 0) {
		return GodotCorpse::from_deserialized(
			CorpseResponseDSI::get_corpse(to_bytes(payload)),
			id);
	}

	inline godot::PackedByteArray build_get_corpse_request(std::uint64_t id) {
		return to_packed_byte_array(FrameMotor::build(BinarySubTypes::Request::GET_CORPSE,
		                                              Serializer::convert_uint64_t(id)));
	}

} // namespace GodotBinaryParser
