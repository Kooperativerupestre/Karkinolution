#pragma once

#include "binary/frame.hpp"
#include "model/creature/creature.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature.hpp>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/frames/parser.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/binary/serialization/serializer.hpp>
#include <vector>

namespace GodotBinaryParser {

	inline std::vector<std::byte> to_bytes(const godot::PackedByteArray &array) {
		std::vector<std::byte> bytes(array.size());
		if (!array.is_empty()) {
			std::memcpy(bytes.data(), array.ptr(), array.size());
		}
		return bytes;
	}

	inline godot::PackedByteArray to_packed_byte_array(const std::vector<std::byte> &bytes) {
		godot::PackedByteArray array;
		array.resize(static_cast<std::int64_t>(bytes.size()));
		if (!bytes.empty()) {
			std::memcpy(array.ptrw(), bytes.data(), bytes.size());
		}
		return array;
	}

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
