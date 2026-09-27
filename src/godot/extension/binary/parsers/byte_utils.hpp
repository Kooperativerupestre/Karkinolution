#pragma once

#include <cstdint>
#include <cstring>
#include <godot_cpp/variant/packed_byte_array.hpp>
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

} // namespace GodotBinaryParser
