#pragma once

#include "binary/frame.hpp"
#include "binary/parsers/byte_utils.hpp"
#include "model/creature/creature.hpp"

#include <cstdint>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>

namespace GodotBinaryParser {

	inline godot::Ref<GodotParsedFrame> parse_frame(const godot::PackedByteArray &bytes) {
		return GodotParsedFrame::from_core(FrameParser::parse_frame(to_bytes(bytes)));
	}

	inline godot::Ref<GodotCreature> parse_creature(const godot::PackedByteArray &payload,
													std::uint64_t                 id = 0) {
		return GodotCreature::from_deserialized(
			CreatureResponseDSI::get_creature(to_bytes(payload)),
			id);
	}

	inline godot::PackedByteArray build_get_creature_request(std::uint64_t id) {
		return to_packed_byte_array(FrameMotor::build(BinarySubTypes::Request::GET_CREATURE,
													  Serializer::convert_uint64_t(id)));
	}

} // namespace GodotBinaryParser
