#include "karkinolution/networking/frame/system/message_interpreters/system.hpp"

#include "shared/binary/byte_utils.hpp"
#include "shared/binary/deserializer.hpp"
#include "shared/binary/serializer.hpp"

namespace Karkinolution::Networking::Frames::System::MessageInterpreters {
	namespace SystemCall {
		std::array<std::byte, SRI::Layout::Config::TOTAL_SIZE>
		SRI::make_config(Karkinolution::World::WorldId world_id) {
			std::array<std::byte, SRI::Layout::Config::TOTAL_SIZE> bytes{};
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(world_id),
												 SRI::Layout::Config::WorldId::OFFSET);
			return bytes;
		}

		Interpreter::WorldPreset::SRI::Bytes SRI::make_create_world(const Karkinolution::World::WorldPreset &preset) {
			return Interpreter::WorldPreset::SRI::serialize(preset);
		}

		Karkinolution::World::WorldId DSI::get_config(const std::vector<std::byte> &bytes, std::size_t offset) {
			return KShared::Binary::Deserializer::deserialize<Karkinolution::World::WorldId>(
				bytes,
				offset + SRI::Layout::Config::WorldId::OFFSET);
		}

		Karkinolution::World::WorldPreset DSI::get_create_world(const std::vector<std::byte> &bytes,
																std::size_t                   offset) {
			return Interpreter::WorldPreset::DSI::deserialize(bytes, offset);
		}
	} // namespace SystemCall

	namespace DSI {
		Models::WorldCreatedOutput get_world_created(const std::vector<std::byte> &bytes, std::size_t offset) {
			return {.world_created = KShared::Binary::Deserializer::read_bool(bytes, offset)};
		}
	} // namespace DSI

	namespace SystemOutput::SRI {
		std::array<std::byte,
				   ::Karkinolution::Networking::Frames::System::Interpreter::SRI::Layout::ConfigOutput::TotalSize>
		make_config_output(const ConfigOutput &output) {
			return Interpreter::SRI::serialize(output);
		}

		std::array<std::byte, sizeof(bool)> make_world_created(const Models::WorldCreatedOutput &output) {
			std::array<std::byte, sizeof(bool)> bytes{};
			KShared::Binary::Utils::append_bytes(bytes,
												 KShared::Binary::Serializer::serialize(output.world_created),
												 0);
			return bytes;
		}
	} // namespace SystemOutput::SRI
} // namespace Karkinolution::Networking::Frames::System::MessageInterpreters
