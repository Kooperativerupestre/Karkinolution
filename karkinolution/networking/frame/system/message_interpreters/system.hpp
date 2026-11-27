#pragma once

#include "karkinolution/networking/frame/system/interpreter.hpp"
#include "karkinolution/networking/frame/system/message_interpreters/models.hpp"
#include "shared/binary/fields.hpp"

#include <array>
#include <vector>

namespace Karkinolution::Networking::Frames::System::MessageInterpreters {
	namespace SystemCall {
		namespace SRI {
			namespace Layout {
				namespace Config {
					using WorldId = KShared::Binary::Field<0, sizeof(Karkinolution::World::WorldId)>;
					inline constexpr std::size_t TOTAL_SIZE = WorldId::END;
				} // namespace Config

				namespace CreateWorld {
					using Payload = Interpreter::WorldPreset::SRI::Bytes;
				} // namespace CreateWorld
			} // namespace Layout

			std::array<std::byte, Layout::Config::TOTAL_SIZE> make_config(Karkinolution::World::WorldId world_id);
			Interpreter::WorldPreset::SRI::Bytes make_create_world(const Karkinolution::World::WorldPreset &preset);
		} // namespace SRI

		namespace DSI {
			Karkinolution::World::WorldId     get_config(const std::vector<std::byte> &bytes, std::size_t offset = 0);
			Karkinolution::World::WorldPreset get_create_world(const std::vector<std::byte> &bytes,
															   std::size_t                   offset = 0);
		} // namespace DSI
	} // namespace SystemCall

	namespace DSI {
		Models::WorldCreatedOutput get_world_created(const std::vector<std::byte> &bytes, std::size_t offset = 0);
	} // namespace DSI

	namespace SystemOutput::SRI {
		std::array<std::byte,
				   ::Karkinolution::Networking::Frames::System::Interpreter::SRI::Layout::ConfigOutput::TotalSize>
											make_config_output(const ConfigOutput &output);
		std::array<std::byte, sizeof(bool)> make_world_created(const Models::WorldCreatedOutput &output);
	} // namespace SystemOutput::SRI
} // namespace Karkinolution::Networking::Frames::System::MessageInterpreters
