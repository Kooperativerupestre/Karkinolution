#pragma once

#include "karkinolution/global_aliasses.hpp"
#include "karkinolution/networking/connection/connection.hpp"
#include "karkinolution/networking/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/networking/simulation/simulation.hpp"
#include "karkinolution/world/world_storage.hpp"
#include "shared/core/error.hpp"
#include "shared/core/id.hpp"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/signal_set.hpp>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/port.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking::Server {

	class Server {
		public:

			Server(IoContext &io_context, Port port, Karkinolution::World::WorldStorage &w);


			~Server();
			void stop();

		private:

			std::vector<std::unique_ptr<Connection::Connection>> connections;

			void                                             accept();
			void                                             remove_connection(Connection::Connection &connection);
			Karkinolution::World::WorldStorage              &world_storage_;
			Simulation::Simulation                           simulation_;
			Acceptor                                         acceptor_;
			IoContext                                       &io_context;
			asio::signal_set                                 signals_;
	};

} // namespace Karkinolution::Networking::Server
