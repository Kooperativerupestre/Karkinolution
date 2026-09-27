#pragma once

#include "binary/parsers/byte_utils.hpp"
#include "godot/extension/model/math/geometry/geometry.hpp"

#include <cstddef>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/geometry/geometry.hpp>

namespace GodotBinaryParser {

	inline godot::Ref<GodotRadius> parse_radius(const godot::PackedByteArray &payload,
												std::size_t                   offset = 0) {
		const auto core_val = GeometryDSI::deserialize_radius(to_bytes(payload), offset);
		return GodotRadius::create(core_val.value);
	}

	inline godot::Ref<GodotCircumference> parse_circumference(const godot::PackedByteArray &payload,
															  std::size_t offset = 0) {
		const auto core_val = GeometryDSI::deserialize_circumference(to_bytes(payload), offset);
		return GodotCircumference::create(core_val.value);
	}

	inline godot::Ref<GodotDiameter> parse_diameter(const godot::PackedByteArray &payload,
													std::size_t                   offset = 0) {
		const auto core_val = GeometryDSI::deserialize_diameter(to_bytes(payload), offset);
		return GodotDiameter::create(core_val.value);
	}

	inline godot::Ref<GodotArea> parse_area(const godot::PackedByteArray &payload,
											std::size_t                   offset = 0) {
		const auto core_val = GeometryDSI::deserialize_area(to_bytes(payload), offset);
		return GodotArea::create(core_val.value);
	}

} // namespace GodotBinaryParser
