#pragma once

#include "binary/frame.hpp"
#include "godot/extension/model/math/stats/limited_value.hpp"
#include "godot/extension/model/math/stats/stats.hpp"
#include "godot/extension/model/math/unit/units.hpp"
#include "model/creature/creature.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <karkinolution/binary/deserialization/interpreters/creature.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/unit/unit.hpp>
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

	inline godot::Ref<GodotVolume> parse_volume(const godot::PackedByteArray &payload,
												std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_volume(to_bytes(payload), offset);
		return GodotVolume::create(core_val.value);
	}

	inline godot::Ref<GodotMass> parse_mass(const godot::PackedByteArray &payload,
											std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_mass(to_bytes(payload), offset);
		return GodotMass::create(core_val.value);
	}

	inline godot::Ref<GodotDensity> parse_density(const godot::PackedByteArray &payload,
												  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_density(to_bytes(payload), offset);
		return GodotDensity::create(core_val.value);
	}

	inline godot::Ref<GodotMeter> parse_meter(const godot::PackedByteArray &payload,
											  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_meter(to_bytes(payload), offset);
		return GodotMeter::create(core_val.value);
	}

	inline godot::Ref<GodotLateral> parse_lateral(const godot::PackedByteArray &payload,
												  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_lateral(to_bytes(payload), offset);
		return GodotLateral::create(core_val.value);
	}

	inline godot::Ref<GodotHeight> parse_height(const godot::PackedByteArray &payload,
												std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_height(to_bytes(payload), offset);
		return GodotHeight::create(core_val.value);
	}

	inline godot::Ref<GodotDepth> parse_depth(const godot::PackedByteArray &payload,
											  std::size_t                   offset = 0) {
		const auto core_val = PhysicsUnitsDSI::deserialize_depth(to_bytes(payload), offset);
		return GodotDepth::create(core_val.value);
	}

	inline godot::Ref<GodotSize> parse_size(const godot::PackedByteArray &payload,
											std::size_t                   offset = 0) {
		const auto core_size = PhysicsUnitsDSI::deserialize_size(to_bytes(payload), offset);
		return GodotSize::from_values(core_size.lateral.value,
									  core_size.height.value,
									  core_size.depth.value);
	}

	inline godot::Ref<GodotSharedVolume> parse_shared_volume(const godot::PackedByteArray &payload,
															 std::size_t                   offset = 0) {
		const auto core_val =
			PhysicsUnitsDSI::deserialize_shared_volume(to_bytes(payload), offset);
		return GodotSharedVolume::create(core_val.value());
	}

	inline godot::Ref<GodotEfficiency> parse_efficiency(const godot::PackedByteArray &payload,
														std::size_t                   offset = 0) {
		const auto core_val =
			PhysicsUnitsDSI::deserialize_efficiency(to_bytes(payload), offset);
		return GodotEfficiency::create(static_cast<double>(core_val.value()));
	}

	inline godot::Ref<GodotQuality> parse_quality(const godot::PackedByteArray &payload,
												  std::size_t                   offset = 0) {
		const auto core_val =
			PhysicsUnitsDSI::deserialize_quality(to_bytes(payload), offset);
		return GodotQuality::create(static_cast<double>(core_val.value()));
	}

} // namespace GodotBinaryParser
