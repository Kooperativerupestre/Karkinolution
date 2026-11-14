#pragma once

#include "shared/core/id.hpp"
#include "shared/functional/unique_gift.hpp"

#include <atomic>
#include <karkinolution/global_aliasses.hpp>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/connection/protocol_sessions/server.hpp>
#include <karkinolution/networking/connection/protocol_sessions/session_manager.hpp>
#include <karkinolution/networking/simulation/simulation.hpp>
#include <karkinolution/world/world.hpp>
#include <mutex>
#include <optional>
#include <vector>

namespace Karkinolution::Networking::Connection {


	struct SendingState {
		public:

			bool is_sending = false;

			std::vector<std::byte> bytes;
	};

	class Connection {
		public:

			Connection(Socket socket, Simulation::Simulation &simulation);

			void                                        start();
			std::move_only_function<void(Connection &)> on_closed;
			void                                        close();

			ConnectionId get_id() const {
				return id;
			}


		private:

			void receive();
			void send_next();
			void send(const std::vector<std::byte> &bytes);
			void on_error(const std::string &error_message);

			inline static std::atomic<ConnectionId> next_id = 0;

			ConnectionId                         id;
			SimulationAdapter::SimulationAdapter simulation_adapter;
			void                                 clear_sending_state();

			ProtocolSessions::SessionManager protocol_session;
			Socket                           socket_;

			std::optional<GiftLease<World::World>> world_ = std::nullopt;

			std::array<std::byte, RESPONSE_QUEUE_MAX_SIZE> temporary_requests;
			SendingState                                   sending_state;
	};

} // namespace Karkinolution::Networking::Connection
