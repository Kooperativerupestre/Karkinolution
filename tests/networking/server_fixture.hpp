#pragma once
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"

#include <gtest/gtest.h>
#include <karkinolution/networking/asio_types.hpp>

class FakeServer {
	public:

		FakeServer()
			: acceptor_(io_, EndPoint(asio::ip::address_v4::loopback(), 0)) {}

		std::uint16_t port() const {
			return acceptor_.local_endpoint().port();
		}

		void accept_peer() {
			peer_.emplace(io_);
			acceptor_.accept(*peer_);
		}

		void drop_conection() {
			peer_->close();
		}

	private:

		IoContext             io_;
		Acceptor              acceptor_;
		std::optional<Socket> peer_;
};

class ClientTest : public ::testing::Test {
	protected:

		FakeServer server;

		asio::io_context                                           io;
		asio::executor_work_guard<asio::io_context::executor_type> guard{io.get_executor()};
		std::thread                                                runner{[this] {
            io.run();
        }};

		~ClientTest() override {
			guard.reset();
			runner.join();
		}
};