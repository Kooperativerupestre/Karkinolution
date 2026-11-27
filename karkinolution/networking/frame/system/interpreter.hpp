#pragma once

#include "karkinolution/networking/config_output.hpp"
#include "karkinolution/networking/world_state.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/binary/fields.hpp"
#include "shared/binary/interpreters/unit.hpp"

#include <array>
#include <cstddef>
#include <vector>

namespace Karkinolution::Networking::Frames::System::Interpreter {
	namespace SRI {
		std::vector<std::byte> serialize(const std::vector<Karkinolution::World::WorldId> &world_ids);

		namespace Layout {
			namespace WorldState {
				using HasOwnerId = KShared::Binary::Field<0, 1>;
				using OwnerId    = KShared::Binary::Next<HasOwnerId, sizeof(Karkinolution::World::WorldId)>;

				inline constexpr std::size_t TotalSize = OwnerId::END;
			} // namespace WorldState

			namespace ConfigOutput {
				using CouldConnect = KShared::Binary::Field<0, 1>;

				inline constexpr std::size_t TotalSize = CouldConnect::END;
			} // namespace ConfigOutput
		} // namespace Layout

		std::array<std::byte, Layout::WorldState::TotalSize>   serialize(const WorldState &world_state);
		std::array<std::byte, Layout::ConfigOutput::TotalSize> serialize(const ConfigOutput &output);
	} // namespace SRI

	namespace DSI {
		WorldState get_world_state(const std::vector<std::byte> &bytes, std::size_t offset = 0);
		std::vector<Karkinolution::World::WorldId> get_world_ids(const std::vector<std::byte> &bytes,
																 std::size_t                   offset = 0);
		ConfigOutput get_config_output(const std::vector<std::byte> &bytes, std::size_t offset = 0);
	} // namespace DSI

	namespace WorldPreset {
		// This binary model is the stable wire representation, not a replacement for the domain preset.
		namespace SRI {
			namespace Layout {
				using Seed                              = KShared::Binary::Field<0, sizeof(int)>;
				using Size                              = KShared::Binary::Next<Seed, UnitsSRI::Layout::TOTAL_SIZE>;
				using Epsilon                           = KShared::Binary::Next<Size, sizeof(float)>;
				inline constexpr std::size_t TOTAL_SIZE = Epsilon::END;
			} // namespace Layout

			using Bytes = std::array<std::byte, Layout::TOTAL_SIZE>;
			Bytes serialize(const Karkinolution::World::WorldPreset &preset);
		} // namespace SRI

		namespace DSI {
			Karkinolution::World::WorldPreset deserialize(const std::vector<std::byte> &bytes, std::size_t offset = 0);
		} // namespace DSI
	} // namespace WorldPreset

} // namespace Karkinolution::Networking::Frames::System::Interpreter
