#include "karkinolution/core/error.hpp"
#include "karkinolution/networking/sdk/src/builders/creature/creature.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/networking/sdk/src/error.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <networking/server_fixture.hpp>

TEST_F(ClientTest, PendingRequestFailsWhenServerDrops) {
	KarkinolutionClient client{
		io,
		KarkinolutionSDK::Client::Config{.host = "127.0.0.1", .port = server.port()}};
	client.connect();
	server.accept_peer();

	const auto id =
		client.send_without_waiting(KarkinolutionSDK::Builders::Creature::see_creature(45));
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
