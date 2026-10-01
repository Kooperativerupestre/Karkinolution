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

namespace CreatureGodotBinaryParser {

	inline godot::Ref<GodotParsedFrame> parse_frame(const godot::PackedByteArray &bytes) {
		return GodotParsedFrame::from_core(
			FrameParser::parse_frame(GodotBinaryParser::to_bytes(bytes)));
	}

	inline godot::Ref<GodotCreature> parse_creature(const godot::PackedByteArray &payload,
													std::uint64_t                 id = 0) {
		return GodotCreature::from_deserialized(
			CreatureResponseDSI::get_creature(GodotBinaryParser::to_bytes(payload)),
			id);
	}

	inline godot::PackedByteArray build_see_creature_request(std::uint64_t id) {
		return GodotBinaryParser::to_packed_byte_array(
			FrameMotor::build(BinarySubTypes::Request::SEE,
							  RequestResource::CREATURE,
							  Serializer::convert_uint64_t(id)));
	}

} // namespace CreatureGodotBinaryParser
