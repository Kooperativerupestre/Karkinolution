#include "karkinolution/networking/frame/system/interpreter.hpp"

#include "karkinolution/networking/connection_id.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/interpreters/unit.hpp"
#include "shared/binary/serializer.hpp"
#include "shared/binary/types/list.hpp"

#include <optional>

namespace Karkinolution::Networking::Frames::System::Interpreter {
	namespace WorldPreset {
		SRI::Bytes SRI::serialize(const Karkinolution::World::WorldPreset &preset) {
			Bytes bytes{};
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(preset.seed),
												 SRI::Layout::Seed::OFFSET);
			KShared::Binary::Utils::append_bytes(bytes,
												 UnitsSRI::serialize_size(preset.size),
												 SRI::Layout::Size::OFFSET);
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::convert_float(preset.epsilon),
												 SRI::Layout::Epsilon::OFFSET);
			return bytes;
		}

		Karkinolution::World::WorldPreset DSI::deserialize(const std::vector<std::byte> &bytes, std::size_t offset) {
			return Karkinolution::World::WorldPreset{
				.seed    = KShared::Binary::Deserializer::deserialize<int>(bytes, offset + SRI::Layout::Seed::OFFSET),
				.size    = UnitsDSI::deserialize_size(bytes, offset + SRI::Layout::Size::OFFSET),
				.epsilon = KShared::Binary::Deserializer::read_float(bytes, offset + SRI::Layout::Epsilon::OFFSET)};
		}
	} // namespace WorldPreset

	namespace SRI {
		std::vector<std::byte> serialize(const std::vector<Karkinolution::World::WorldId> &world_ids) {
			return KShared::Binary::Types::List::SRI::serialize<sizeof(Karkinolution::World::WorldId)>(
				world_ids,
				[](const Karkinolution::World::WorldId id) {
					return KShared::Binary::Serializer::serialize(id);
				});
		}

		std::array<std::byte, Layout::WorldState::TotalSize> serialize(const WorldState &state) {
			std::array<std::byte, Layout::WorldState::TotalSize> bytes{};
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

		std::array<std::byte, Layout::ConfigOutput::TotalSize> serialize(const ConfigOutput &output) {
			std::array<std::byte, Layout::ConfigOutput::TotalSize> bytes{};
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(output.could_connect),
												 Layout::ConfigOutput::CouldConnect::OFFSET);
			return bytes;
		}
	} // namespace SRI

	namespace DSI {
		WorldState get_world_state(const std::vector<std::byte> &bytes, std::size_t offset) {
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

		std::vector<Karkinolution::World::WorldId> get_world_ids(const std::vector<std::byte> &bytes,
																 std::size_t                   offset) {
			return KShared::Binary::Types::List::DSI::deserialize<sizeof(Karkinolution::World::WorldId)>(
				bytes,
				[offset](const std::vector<std::byte> &payload, std::size_t item_offset) {
					return KShared::Binary::Deserializer::deserialize<Karkinolution::World::WorldId>(payload,
																									 item_offset
																										 + offset);
				},
				0);
		}

		ConfigOutput get_config_output(const std::vector<std::byte> &bytes, std::size_t offset) {
			return {.could_connect = KShared::Binary::Deserializer::deserialize<bool>(
						bytes,
						offset + SRI::Layout::ConfigOutput::CouldConnect::OFFSET)};
		}
	} // namespace DSI

} // namespace Karkinolution::Networking::Frames::System::Interpreter
