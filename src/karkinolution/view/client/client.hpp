#pragma once


#include "karkinolution/networking/sdk/src/client/client.hpp"

namespace KarkinolutionView {
	namespace Client {
		class InternalClientGuard {
			public:

				KarkinolutionSDK::Client::Client &client;

				InternalClientGuard(KarkinolutionSDK::Client::Client &client)
					: client(client) {}

				~InternalClientGuard() = default;

				void finish();
		};

		class Client {
			private:

				asio::io_context                                           io;
				asio::executor_work_guard<asio::io_context::executor_type> guard{io.get_executor()};
				std::thread                                                runner{[this] {
                    io.run();
                }};
				std::optional<KarkinolutionSDK::Client::Client>            client;

			public:

				Client()
					: client(std::in_place, io)
					, runner([this] {
						io.run();
					}) {}

				~Client() {
					client.reset();
					guard.reset();
					runner.join();
				}

				bool                is_connected();
				bool                connect();
				bool                disconnect();
				InternalClientGuard get_client_guard();
		};
	} // namespace Client
} // namespace KarkinolutionView
