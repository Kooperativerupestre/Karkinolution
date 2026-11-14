#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/networking/client_side/client/client.hpp"

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <optional>
#include <thread>

namespace KarkinolutionView {
	namespace Client {
		class Client {
			private:

				asio::io_context                                                     io;
				asio::executor_work_guard<asio::io_context::executor_type>           guard{io.get_executor()};
				std::thread                                                          runner{[this] {
                    io.run();
                }};
				std::optional<Karkinolution::Networking::ClientSide::Client::Client> client;

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

				bool is_connected() const;
				bool connect();
				bool disconnect();
				Karkinolution::Networking::ClientSide::Client::ResponseResult
				request(const Karkinolution::Networking::ClientSide::Client::RequestData &data);
		};
	} // namespace Client
} // namespace KarkinolutionView
