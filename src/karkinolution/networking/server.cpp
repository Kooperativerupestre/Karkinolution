#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/port.hpp"

#include <asio/write.hpp>
#include <csignal>
#include <iomanip>
#include <iostream>
#include <karkinolution/networking/server.hpp>
#include <span>

void Server::stop() {
	std::cout << "Stopping server...\n";


	acceptor_.close();
	for (auto &connection : connections) {
		connection->close();
	}


	connections.clear();


	io_context.stop();
}

Server::Server(IoContext &io_context, Port port, World &world)
	: world(world)
	, io_context(io_context)
	, acceptor_(io_context)
	, signals_(io_context, SIGINT, SIGTERM) {

	log_receive_overflow_mode();

	acceptor_.open(asio::ip::tcp::v4());
	acceptor_.bind(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port));

	acceptor_.listen();
	signals_.async_wait([this](const AsioErrorCode &error, int signal) {
		if (error) {
			return;
		}

		std::cout << "\nReceived signal " << signal << ". Stopping server...\n";

		stop();
	});

	accept();
}

void Server::remove_connection(Connection &dead_connection) {
	auto it = std::find_if(connections.begin(),
						   connections.end(),
						   [&dead_connection](const std::unique_ptr<Connection> &connection) {
							   return connection.get() == &dead_connection;
						   });

	if (it != connections.end()) {
		connections.erase(it);
	}
}

void Server::accept() {
	acceptor_.async_accept([this](const AsioErrorCode &error, Socket socket) {
		if (error) {
			return;
		}

		std::cout << "Connected 🦀\n";

		auto connection = std::make_unique<Connection>(std::move(socket), world);

		connection->on_closed = [this](Connection &connection) {
			asio::post(acceptor_.get_executor(), [this, &connection] {
				remove_connection(connection);
			});
		};

		connection->start();

		connections.push_back(std::move(connection));

		if (connections.size() > 0) {
			return;
		}
		accept();
	});
}

Server::~Server() {
	std::cout << "Server was killed 🦀\n";
}