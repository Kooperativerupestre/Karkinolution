#include "karkinolution/api/creature.hpp"
#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/networking/client_side/interpreters/creature.hpp"
#include "karkinolution/networking/client_side/interpreters/soil.hpp"
#include "karkinolution/networking/client_side/interpreters/system.hpp"
#include "karkinolution/networking/client_side/world/commanders/creature.hpp"
#include "karkinolution/networking/port.hpp"
#include "karkinolution/networking/server_side/server/server.hpp"
#include "karkinolution/test/utils.hpp"
#include "karkinolution/world/world_storage.hpp"

#include <exception>
#include <gtest/gtest.h>
#include <karkinolution/networking/asio_types.hpp>
#include <latch>
#include <networking/server_fixture.hpp>

using Karkinolution::Networking::IoContext;
using Karkinolution::Networking::PORT;
using Karkinolution::Networking::ClientSide::Client::Client;
using Karkinolution::Networking::ClientSide::Client::Config;
using Karkinolution::Networking::ClientSide::World::Commanders::Creature::tick_creature;
using LocalWorld = Karkinolution::Networking::ClientSide::World::World;
using Karkinolution::World::World;
using Karkinolution::World::WorldId;
using Karkinolution::World::WorldStorage;
using ServerType = Karkinolution::Networking::ServerSide::Server::Server;

TEST_F(ClientTest, ServerE2E) {
	IoContext server_io;

	WorldStorage         storage;
	std::vector<WorldId> world_ids;
	world_ids.reserve(8);
	for (int index = 0; index < 8; ++index) {
		World world = TestUtils::make_small_world();
		world_ids.push_back(world.get_id());
		ASSERT_TRUE(storage.add(std::move(world)));
	}
	ServerType application_server{server_io, PORT, storage};

	std::thread server_thread{[&] {
		server_io.run();
	}};

	constexpr int N = 8;

	std::atomic<int>         failures{0};
	std::vector<std::thread> threads;

	for (int i = 0; i < N; ++i) {
		threads.emplace_back([&, world_id = world_ids[i]] {
			Client client{io, Config{.host = "127.0.0.1", .port = PORT}};

			try {
				client.connect();
				EXPECT_TRUE(SystemInterpreter::config(client, world_id).value_or(false));
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
		application_server.stop();
	});

	server_thread.join();

	EXPECT_EQ(failures.load(), 0);
}

TEST_F(ClientTest, CreatureTickReturnsUpdatedCreatureAndUpdatesLocalWorld) {
	IoContext server_io;

	World      server_world = TestUtils::make_small_world();
	const auto created      = Karkinolution::API::Creature::make_creature(server_world,
                                                                     std::nullopt,
                                                                     std::nullopt,
                                                                     std::nullopt,
                                                                     std::nullopt);
	ASSERT_TRUE(created);
	const auto creature_id = created->ontology.id;

	WorldStorage storage;
	ASSERT_TRUE(storage.add(std::move(server_world)));
	ServerType application_server{server_io, PORT, storage};

	std::thread server_thread{[&] {
		server_io.run();
	}};

	LocalWorld local_world;
	bool       succeeded = false;
	{
		Client client{io, Config{.host = "127.0.0.1", .port = PORT}};
		try {
			client.connect();
			ASSERT_TRUE(SystemInterpreter::config(client, storage.get_all_world_ids().front()).value_or(false));
			EXPECT_FALSE(CreatureInterpreter::tick_creature(client, KShared::Core::Id::gen_id()));
			const auto initial = CreatureInterpreter::see_creature(client, creature_id);
			ASSERT_TRUE(initial);
			ASSERT_TRUE(local_world.add(*initial));
			succeeded = tick_creature(client, local_world, creature_id);
		} catch (const std::exception &error) {
			ADD_FAILURE() << "Creature tick failed: " << error.what();
		}
		client.stop_new_async_operations();
		client.wait_until_zero_async_operations();
	}

	asio::post(server_io, [&] {
		application_server.stop();
	});
	server_thread.join();

	EXPECT_TRUE(succeeded);
	EXPECT_TRUE(local_world.has_creature(creature_id));
}

TEST_F(ClientTest, ServerAcceptsNewClientAfterDisconnect) {
	IoContext server_io;

	WorldStorage storage;
	World        world    = TestUtils::make_small_world();
	const auto   world_id = world.get_id();
	ASSERT_TRUE(storage.add(std::move(world)));
	ServerType application_server{server_io, PORT, storage};

	std::thread server_thread{[&] {
		server_io.run();
	}};

	const auto make_request = [&] {
		bool succeeded = false;

		{
			Client client{io, Config{.host = "127.0.0.1", .port = PORT}};

			try {
				client.connect();
				EXPECT_TRUE(SystemInterpreter::config(client, world_id).value_or(false));
				auto output = client.request(TestUtils::make_see_creature_request_data(45));
				succeeded   = output && output.value().has_value();
			} catch (const std::exception &error) {
				ADD_FAILURE() << "Client request failed: " << error.what();
			} catch (...) {
				ADD_FAILURE() << "Client request failed with an unknown exception";
			}

			try {
				client.stop_new_async_operations();
				client.wait_until_zero_async_operations();
			} catch (const std::exception &error) {
				ADD_FAILURE() << "Client shutdown failed: " << error.what();
				succeeded = false;
			}
		}

		return succeeded;
	};

	const bool first_request_succeeded  = make_request();
	const bool second_request_succeeded = make_request();

	asio::post(server_io, [&] {
		application_server.stop();
	});

	server_thread.join();

	EXPECT_TRUE(first_request_succeeded);
	EXPECT_TRUE(second_request_succeeded);
}

TEST_F(ClientTest, ClientReceivesLargeSoilListResponse) {
	IoContext server_io;

	WorldStorage storage;
	World        world               = TestUtils::make_small_world();
	const auto   world_id            = world.get_id();
	const auto   expected_soil_count = world.see_territory().registry().size();
	ASSERT_TRUE(storage.add(std::move(world)));
	ServerType application_server{server_io, PORT, storage};

	std::thread server_thread{[&] {
		server_io.run();
	}};

	std::size_t soil_count = 0;
	bool        succeeded  = false;
	{
		Client client{io, Config{.host = "127.0.0.1", .port = PORT}};

		try {
			client.connect();
			const auto world_ids = SystemInterpreter::get_all_world_ids(client);
			EXPECT_EQ(world_ids, std::vector<Karkinolution::World::WorldId>{world_id});
			EXPECT_TRUE(SystemInterpreter::config(client, world_id).value_or(false));
			soil_count = SoilInterpreter::see_all_soils(client).size();
			succeeded  = true;
		} catch (const std::exception &error) {
			ADD_FAILURE() << "Fetching all soils failed: " << error.what();
		}

		client.stop_new_async_operations();
		client.wait_until_zero_async_operations();
	}

	asio::post(server_io, [&] {
		application_server.stop();
	});

	server_thread.join();

	ASSERT_TRUE(succeeded);
	EXPECT_EQ(soil_count, expected_soil_count);
}
