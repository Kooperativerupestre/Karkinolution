#pragma once

#include "karkinolution/binary/frames/processor.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/networking/protocol_session.hpp"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/signal_set.hpp>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/connection.hpp>
#include <karkinolution/networking/port.hpp>
#include <karkinolution/world/world.hpp>

class Server {
	public:

		Server(IoContext &io_context, Port port, World &w);

		const World &world_view() const {
			return world;
		}

		~Server();
		void stop();

	private:

		std::vector<std::unique_ptr<Connection>> connections;

		void             accept();
		void             remove_connection(Connection &connection);
		World           &world;
		Acceptor         acceptor_;
		IoContext       &io_context;
		asio::signal_set signals_;
};
