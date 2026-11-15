#include "karkinolution/networking/client_side/client/alias.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/test/utils.hpp"
#include "shared/core/error.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <networking/server_fixture.hpp>
#include <thread>

using Karkinolution::Networking::ClientSide::ClientError;

TEST_F(ClientTest, PendingRequestFailsWhenServerDrops) {
	KarkinolutionClient client{
		io,
		Karkinolution::Networking::ClientSide::Client::Config{.host = "127.0.0.1", .port = server.port()}};
	client.connect();
	server.accept_peer();

	const auto id = client.send_without_waiting(TestUtils::make_see_creature_request_data(45));
	server.drop_conection();
	const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
	bool       failed   = false;
	while (std::chrono::steady_clock::now() < deadline) {
		try {
			const auto output = client.pop_response(id);
		} catch (const ClientError &error) {
			failed = true;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	EXPECT_TRUE(failed);
}
