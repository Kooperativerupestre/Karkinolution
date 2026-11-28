#include "karkinolution/networking/server_side/server/server.hpp"

#include "karkinolution/global_aliasses.hpp"
#include "karkinolution/logger.hpp"
#include "karkinolution/networking/port.hpp"
#include "karkinolution/networking/server_side/connection/connection.hpp"
#include "karkinolution/world/world_storage.hpp"
#include "shared/core/id.hpp"

#include <algorithm>
#include <asio/write.hpp>
#include <csignal>
#include <format>
#include <iomanip>
#include <iostream>
#include <span>

namespace Karkinolution::Networking::ServerSide::Server {
	void Server::stop() {
		std::cout << "Stopping server...\n";

		acceptor_.close();
		for (auto &connection : connections) {
			connection->close();
		}

		connections.clear();

		io_context.stop();
	}

	Server::Server(IoContext &io_context, Port port, Karkinolution::World::WorldStorage &world)
		: world_storage_(world)
		, simulation_(world_storage_)
		, io_context(io_context)
		, acceptor_(io_context)
		, signals_(io_context, SIGINT, SIGTERM) {
		logger.log<LogLevel::INFO>("Karkinolution server is initializing 🦀");
		logger.log<LogLevel::INFO>(std::format("Port: {}", port));

		logger.log<LogLevel::DEBUG>("Opening protocol tcp v4");
		acceptor_.open(asio::ip::tcp::v4());

		const auto reuse_address = true;

		if (reuse_address) {
			logger.log<LogLevel::INFO>("Reuse address: true");
		} else {
			logger.log<LogLevel::INFO>("Reuse address: false");
		}
		acceptor_.set_option(asio::socket_base::reuse_address(true));


		logger.log<LogLevel::DEBUG>("Binding acceptor...");
		acceptor_.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));

		logger.log<LogLevel::DEBUG>("Listening...");
		acceptor_.listen();
		signals_.async_wait([this](const AsioErrorCode &error, int signal) {
			if (error) {
				return;
			}

			logger.log<LogLevel::INFO>(std::format("\nReceived signal {}.", signal));

			stop();
		});

		accept();
	}

	void Server::remove_connection(Connection::Connection &dead_connection) {
		auto it = std::find_if(connections.begin(),
							   connections.end(),
							   [&dead_connection](const std::unique_ptr<Connection::Connection> &connection) {
								   return connection.get() == &dead_connection;
							   });

		if (it != connections.end()) {
			logger.log<LogLevel::INFO>(
				std::format("Connection with id = {} has been removed", dead_connection.get_id()));
			connections.erase(it);
		}
	}

	void Server::accept() {
		acceptor_.async_accept([this](const AsioErrorCode &error, Socket socket) {
			if (error) {
				return;
			}

			logger.log<LogLevel::INFO>("Accepted new connection");

			auto connection = std::make_unique<Connection::Connection>(std::move(socket), simulation_);

			logger.log<LogLevel::INFO>(std::format("Connection created with id = {}", connection->get_id()));

			connection->on_closed = [this](Connection::Connection &connection) {
				asio::post(acceptor_.get_executor(), [this, &connection] {
					remove_connection(connection);
				});
			};

			connection->start();

			connections.push_back(std::move(connection));

			accept();
		});
	}

	Server::~Server() {
		logger.log<LogLevel::INFO>("Server was killed🦀");
	}
} // namespace Karkinolution::Networking::ServerSide::Server
