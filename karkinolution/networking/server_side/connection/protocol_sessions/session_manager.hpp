#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/scope/layout.hpp"
#include "karkinolution/networking/server_side/connection/protocol_sessions/server.hpp"
#include "karkinolution/networking/server_side/connection/protocol_sessions/world.hpp"
#include "karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp"

#include <optional>
#include <span>
#include <variant>

namespace Karkinolution::Networking::ServerSide::Connection::ProtocolSessions {
	using SessionResponse = std::variant<Frames::Models::SystemOutput::SystemOutput, ResponseFrame>;

	class SessionManager {
		private:

			SimulationAdapter::SimulationAdapter &simulation;
			ServerSession                         server_session;
			std::optional<WorldSession>           world_session;
			RawFrameBuffer                        incoming;

			void process_frames();

		public:

			explicit SessionManager(SimulationAdapter::SimulationAdapter &simulation)
				: simulation(simulation)
				, server_session(simulation) {}

			void            receive(std::span<const std::byte> bytes);
			bool            has_response() const;
			SessionResponse pop_response();
	};
} // namespace Karkinolution::Networking::ServerSide::Connection::ProtocolSessions
