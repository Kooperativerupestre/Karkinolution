#include "karkinolution/binary/types/list.hpp"
#include "karkinolution/networking/config_output.hpp"
#include "karkinolution/networking/connection_id.hpp"
#include "karkinolution/networking/world_state.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/serializer.hpp"

#include <karkinolution/networking/frame/interpreter.hpp>
#include <vector>

namespace Karkinolution {
	namespace Networking::Frames::Interpreter {
		std::vector<std::byte> SRI::serialize(const std::vector<World::WorldId> &world_ids) {
			return Binary::Types::List::SRI::serialize<sizeof(World::WorldId)>(world_ids, [](const World::WorldId id) {
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
			return Binary::Types::List::DSI::deserialize<sizeof(World::WorldId)>(
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

		// OTHERS

		std::array<std::byte, SystemCall::SRI::Layout::Config::TOTAL_SIZE>
		SystemCall::SRI::make_config(World::WorldId world_id) {
			std::array<std::byte, SystemCall::SRI::Layout::Config::TOTAL_SIZE> bytes{};
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(world_id),
												 SystemCall::SRI::Layout::Config::WorldId::OFFSET);
			return bytes;
		}

		World::WorldId SystemCall::DSI::get_config(const std::vector<std::byte> &bytes, std::size_t offset) {
			return KShared::Binary::Deserializer::deserialize<World::WorldId>(
				bytes,
				offset + SystemCall::SRI::Layout::Config::WorldId::OFFSET);
		}

		std::array<std::byte, SRI::Layout::ConfigOutput::TotalSize>
		SystemOutput::SRI::make_config_output(const ConfigOutput &output) {
			return Interpreter::SRI::serialize(output);
		}
	} // namespace Networking::Frames::Interpreter
} // namespace Karkinolution