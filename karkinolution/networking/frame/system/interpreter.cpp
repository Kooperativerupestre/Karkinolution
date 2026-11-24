#include "karkinolution/networking/config_output.hpp"
#include "karkinolution/networking/connection_id.hpp"
#include "karkinolution/networking/world_state.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/interpreters/unit.hpp"
#include "shared/binary/serializer.hpp"
#include "shared/binary/types/list.hpp"

#include <karkinolution/networking/frame/system/interpreter.hpp>
#include <vector>

namespace Karkinolution {
	namespace Networking::Frames::System::Interpreter {
		WorldPreset::SRI::Bytes WorldPreset::SRI::serialize(const World::WorldPreset &preset) {
			WorldPreset::SRI::Bytes bytes{};
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(preset.seed),
												 WorldPreset::SRI::Layout::Seed::OFFSET);
			KShared::Binary::Utils::append_bytes(bytes,
												 UnitsSRI::serialize_size(preset.size),
												 WorldPreset::SRI::Layout::Size::OFFSET);
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::convert_float(preset.epsilon),
												 WorldPreset::SRI::Layout::Epsilon::OFFSET);
			return bytes;
		}

		World::WorldPreset WorldPreset::DSI::deserialize(const std::vector<std::byte> &bytes, std::size_t offset) {
			return World::WorldPreset{
				.seed =
					KShared::Binary::Deserializer::deserialize<int>(bytes,
																	offset + WorldPreset::SRI::Layout::Seed::OFFSET),
				.size = UnitsDSI::deserialize_size(bytes, offset + WorldPreset::SRI::Layout::Size::OFFSET),
				.epsilon =
					KShared::Binary::Deserializer::read_float(bytes,
															  offset + WorldPreset::SRI::Layout::Epsilon::OFFSET)};
		}

		std::vector<std::byte> SRI::serialize(const std::vector<World::WorldId> &world_ids) {
			return KShared::Binary::Types::List::SRI::serialize<sizeof(World::WorldId)>(
				world_ids,
				[](const World::WorldId id) {
					return KShared::Binary::Serializer::serialize(id);
				});
		}

		std::array<std::byte, SRI::Layout::WorldState::TotalSize> SRI::serialize(const WorldState &state) {
			std::array<std::byte, SRI::Layout::WorldState::TotalSize> bytes;

			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(!state.is_free()),
												 Layout::WorldState::HasOwnerId::OFFSET);
			if (!state.is_free()) {
				KShared::Binary::Utils::append_bytes(bytes,
													 KShared::Binary::Serializer::serialize(state.owner.value()),
													 Layout::WorldState::OwnerId::OFFSET);
			}
			return bytes;
		}

		std::array<std::byte, SRI::Layout::ConfigOutput::TotalSize> SRI::serialize(const ConfigOutput &output) {
			std::array<std::byte, SRI::Layout::ConfigOutput::TotalSize> bytes{};

			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(output.could_connect),
												 SRI::Layout::ConfigOutput::CouldConnect::OFFSET);
			return bytes;
		}

		WorldState DSI::get_world_state(const std::vector<std::byte> &bytes, std::size_t offset) {
			const bool has_owner_id =
				KShared::Binary::Deserializer::read_bool(bytes, SRI::Layout::WorldState::HasOwnerId::OFFSET + offset);

			std::optional<ConnectionId> id;

			if (has_owner_id) {
				id = KShared::Binary::Deserializer::deserialize<ConnectionId>(bytes,
																			  SRI::Layout::WorldState::OwnerId::OFFSET
																				  + offset);
			}
			return WorldState{.owner = id};
		}

		std::vector<World::WorldId> DSI::get_world_ids(const std::vector<std::byte> &bytes, std::size_t offset) {
			return KShared::Binary::Types::List::DSI::deserialize<sizeof(World::WorldId)>(
				bytes,
				[offset](const std::vector<std::byte> &bytes, std::size_t p_offset) {
					return KShared::Binary::Deserializer::deserialize<World::WorldId>(bytes, p_offset + offset);
				},
				0);
		}

		ConfigOutput DSI::get_config_output(const std::vector<std::byte> &bytes, std::size_t offset) {
			return {.could_connect = KShared::Binary::Deserializer::deserialize<bool>(
						bytes,
						offset + SRI::Layout::ConfigOutput::CouldConnect::OFFSET)};
		}

		DSI::WorldCreatedOutput DSI::get_world_created(const std::vector<std::byte> &bytes, std::size_t offset) {
			return {.world_created = KShared::Binary::Deserializer::read_bool(bytes, offset)};
		}

		// OTHERS

		std::array<std::byte, SystemCall::SRI::Layout::Config::TOTAL_SIZE>
		SystemCall::SRI::make_config(World::WorldId world_id) {
			std::array<std::byte, SystemCall::SRI::Layout::Config::TOTAL_SIZE> bytes{};
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(world_id),
												 SystemCall::SRI::Layout::Config::WorldId::OFFSET);
			return bytes;
		}

		WorldPreset::SRI::Bytes SystemCall::SRI::make_create_world(const World::WorldPreset &preset) {
			return WorldPreset::SRI::serialize(preset);
		}

		World::WorldId SystemCall::DSI::get_config(const std::vector<std::byte> &bytes, std::size_t offset) {
			return KShared::Binary::Deserializer::deserialize<World::WorldId>(
				bytes,
				offset + SystemCall::SRI::Layout::Config::WorldId::OFFSET);
		}

		World::WorldPreset SystemCall::DSI::get_create_world(const std::vector<std::byte> &bytes, std::size_t offset) {
			return WorldPreset::DSI::deserialize(bytes, offset);
		}

		std::array<std::byte, SRI::Layout::ConfigOutput::TotalSize>
		SystemOutput::SRI::make_config_output(const ConfigOutput &output) {
			return Interpreter::SRI::serialize(output);
		}

		std::array<std::byte, sizeof(bool)>
		SystemOutput::SRI::make_world_created(const DSI::WorldCreatedOutput &output) {
			std::array<std::byte, sizeof(bool)> bytes{};
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(output.world_created),
												 0);
			return bytes;
		}
	} // namespace Networking::Frames::System::Interpreter
} // namespace Karkinolution
