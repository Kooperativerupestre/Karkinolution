#include "karkinolution/test/utils.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <networking/server_fixture.hpp>
#include <thread>

TEST_F(ClientTest, SendCompletionReleasesItsAsyncOperation) {
	KarkinolutionClient client{
		io,
		Karkinolution::Networking::ClientSide::Client::Config{.host = "127.0.0.1", .port = server.port()}};
	client.connect();
	server.accept_peer();

	client.send_without_waiting_world_scope(TestUtils::make_see_creature_request_data(45));

	const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
	while (std::chrono::steady_clock::now() < deadline && client.get_current_async_operations_running() != 1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	EXPECT_EQ(client.get_current_async_operations_running(), 1);

	server.drop_conection();
	client.stop_new_async_operations();
	client.wait_until_zero_async_operations();

	EXPECT_EQ(client.get_current_async_operations_running(), 0);
}
