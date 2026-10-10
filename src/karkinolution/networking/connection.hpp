#pragma once
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/protocol_session.hpp>
#include <karkinolution/world/world.hpp>
#include <vector>

struct SendingState {
	public:

		bool is_sending = false;

		std::vector<std::byte> bytes;
};

class Connection {
	public:

		Connection(Socket socket, World &world);

		void                                        start();
		std::move_only_function<void(Connection &)> on_closed;
		void                                        close();

	private:

		void receive();
		void send_next();
		void send(const std::vector<std::byte> &bytes);
		void on_error(const std::string &error_message);


		void clear_sending_state();

		ProtocolSession protocol_session;
		World          &world;
		Socket          socket_;

		std::array<std::byte, RESPONSE_QUEUE_MAX_SIZE> temporary_requests;
		SendingState                                   sending_state;
};