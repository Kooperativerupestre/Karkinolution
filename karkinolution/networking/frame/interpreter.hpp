#pragma once

#include "karkinolution/networking/simulation/config_output.hpp"
#include "karkinolution/networking/simulation/world_state.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/binary/fields.hpp"

#include <array>
#include <cstddef>
#include <vector>

/*
It is worth noting that, in this part of the Server frames implementation, I did not create
specific models for serialization and deserialization, unlike what I did in the simulation
core. The main reason is that there is not enough implementation complexity here (most of
these are small structs) to justify dedicated serialization models. The same principle was
also applied when avoiding the creation of dedicated models for the mathematical model
classes/structs in the Kshared library.
*/


namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Interpreter {

				namespace SRI {
					std::vector<std::byte> serialize(const std::vector<World::WorldId> &world_ids);

					namespace Layout {
						namespace WorldState {
							using HasOwnerId = KShared::Binary::Field<0, 1>;
							using OwnerId    = KShared::Binary::Next<HasOwnerId, sizeof(World::WorldId)>;

							inline constexpr std::size_t TotalSize = OwnerId::END;
						} // namespace WorldState

						namespace ConfigOutput {
							using CouldConnect = KShared::Binary::Field<0, 1>;

							inline constexpr std::size_t TotalSize = CouldConnect::END;
						} // namespace ConfigOutput
					} // namespace Layout

					std::array<std::byte, Layout::WorldState::TotalSize>
					serialize(const Simulation::WorldState &world_state);
					std::array<std::byte, Layout::ConfigOutput::TotalSize>
					serialize(const Simulation::ConfigOutput &output);
				} // namespace SRI

				namespace DSI {


					Simulation::WorldState get_world_state(const std::vector<std::byte> &bytes, std::size_t offset = 0);
					std::vector<World::WorldId> get_world_ids(const std::vector<std::byte> &bytes,
															  std::size_t                   offset = 0);
					Simulation::ConfigOutput    get_config_output(const std::vector<std::byte> &bytes,
																  std::size_t                   offset = 0);
				} // namespace DSI

				namespace SystemCall {
					namespace SRI {
						namespace Layout {
							namespace Config {
								using WorldId = KShared::Binary::Field<0, sizeof(World::WorldId)>;
								inline constexpr std::size_t TOTAL_SIZE = WorldId::END;
							} // namespace Config
						} // namespace Layout

						std::array<std::byte, Layout::Config::TOTAL_SIZE> make_config(World::WorldId world_id);

					} // namespace SRI

					namespace DSI {
						World::WorldId get_config(const std::vector<std::byte> &bytes, std::size_t offset = 0);
					} // namespace DSI
				} // namespace SystemCall

				namespace SystemOutput {
					namespace SRI {


						std::array<std::byte, Interpreter::SRI::Layout::ConfigOutput::TotalSize>
						make_config_output(const Simulation::ConfigOutput &output);
					} // namespace SRI

				} // namespace SystemOutput

			} // namespace Interpreter
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution
