#pragma once

#include "karkinolution/networking/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/response_queue.hpp"
#include "karkinolution/networking/system_output_queue.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/binary/byte_range.hpp"

#include <karkinolution/networking/frame/models.hpp>

namespace Karkinolution::Networking::Connection::ProtocolSessions {
	class ServerSession {
		private:

			SimulationAdapter::SimulationAdapter  &simulation;
			SystemOutputQueue                      system_outputs;
			RawFrameBuffer                         call_buffer;
			std::optional<GiftLease<World::World>> world_lease;

			void on_overflow_error(std::span<const std::byte> bytes) const;
			void process_frames();
			void clear_calls();

		public:

			explicit ServerSession(SimulationAdapter::SimulationAdapter &simulation)
				: simulation(simulation) {}

			void                                       receive(std::span<const std::byte> bytes);
			bool                                       has_response() const;
			Frames::Models::SystemOutput::SystemOutput pop_response();
			bool                                       has_world() const;
			World::World                              &world();
	};
} // namespace Karkinolution::Networking::Connection::ProtocolSessions
