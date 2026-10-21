#include "karkinolution/networking/port.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/networking/server.hpp"
#include "karkinolution/test/utils.hpp"

#include <gtest/gtest.h>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/sdk/src/client/client.hpp>
#include <latch>
#include <networking/server_fixture.hpp>

TEST_F(ClientTest, ServerE2E) {
	IoContext server_io;

	World  world = TestUtils::make_small_world();
	Server server{server_io, PORT, world};

	std::thread server_thread{[&] {
		server_io.run();
	}};

	constexpr int N = 8;

	std::atomic<int>         failures{0};
	std::vector<std::thread> threads;

	for (int i = 0; i < N; ++i) {
		threads.emplace_back([&] {
			KarkinolutionSDK::Client::Client client{
				io,
				KarkinolutionSDK::Client::Config{.host = "127.0.0.1", .port = PORT}};

			try {
				client.connect();
				auto output = client.request(TestUtils::make_see_creature_request_data(45));

				if (!output || !output.value().has_value()) {
					failures.fetch_add(1);
				}
			} catch (...) {
				failures.fetch_add(1);
			}

			try {
				client.stop_new_async_operations();
				client.wait_until_zero_async_operations();
			} catch (...) {
				failures.fetch_add(1);
			}
		});
	}

	for (auto &th : threads) {
		th.join();
	}

	asio::post(server_io, [&] {
		server.stop();
	});

	server_thread.join();

	EXPECT_EQ(failures.load(), 0);
}