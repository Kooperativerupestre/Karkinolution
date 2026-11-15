#include "karkinolution/networking/client_side/client/alias.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/test/utils.hpp"

#include <chrono>
#include <gtest/gtest.h>
#include <networking/server_fixture.hpp>
#include <thread>

TEST(RuntimeClientLifecycle, DestructorCleansUpWithoutHanging) {
	KarkinolutionRuntimeClient client{
		Karkinolution::Networking::ClientSide::Client::Config{.host = "127.0.0.1", .port = 65534}};
	EXPECT_FALSE(client.is_connected());
}

TEST(RuntimeClientLifecycle, ConnectFailsGracefullyWhenServerIsOffline) {
	KarkinolutionRuntimeClient client{
		Karkinolution::Networking::ClientSide::Client::Config{.host = "127.0.0.1", .port = 65534}};
	EXPECT_FALSE(client.connect());
	EXPECT_FALSE(client.is_connected());
	EXPECT_TRUE(client.disconnect());
}

TEST_F(ClientTest, RuntimeClientConnectsAndDisconnectsSafelyWithServer) {
	KarkinolutionRuntimeClient client{
		Karkinolution::Networking::ClientSide::Client::Config{.host = "127.0.0.1", .port = server.port()}};
	EXPECT_FALSE(client.is_connected());
	EXPECT_TRUE(client.connect());
	server.accept_peer();
	EXPECT_TRUE(client.is_connected());

	EXPECT_TRUE(client.disconnect());
	EXPECT_FALSE(client.is_connected());
}
