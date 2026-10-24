#pragma once


#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"

namespace KarkinolutionView {
	namespace Client {


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

				bool                                    is_connected();
				bool                                    connect();
				bool                                    disconnect();
				KarkinolutionSDK::Client::Client       &get_client();
				const KarkinolutionSDK::Client::Client &get_client() const;
		};
	} // namespace Client
} // namespace KarkinolutionView
