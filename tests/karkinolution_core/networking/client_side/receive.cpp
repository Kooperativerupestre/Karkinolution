#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/test/utils.hpp"

#include <array>
#include <chrono>
#include <gtest/gtest.h>
#include <networking/server_fixture.hpp>
#include <thread>

using Karkinolution::Networking::ClientSide::ClientError;

TEST_F(ClientTest, MalformedReceivedBytesReleaseOperationAndFailPendingRequest) {
	KarkinolutionClient client{
		io,
		Karkinolution::Networking::ClientSide::Client::Config{.host = "127.0.0.1", .port = server.port()}};
	client.connect();
	server.accept_peer();

	const auto request_id = client.send_without_waiting_world_scope(TestUtils::make_see_creature_request_data(45));
	const std::array invalid_scope{std::byte{0xff}};
	server.send_bytes(invalid_scope);

	const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
	bool       failed   = false;
	while (std::chrono::steady_clock::now() < deadline) {
		try {
			(void)client.pop_response_world_scope(request_id);
		} catch (const ClientError &) {
			failed = true;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	EXPECT_TRUE(failed);
	client.stop_new_async_operations();
	client.wait_until_zero_async_operations();
	EXPECT_EQ(client.get_current_async_operations_running(), 0);
}
