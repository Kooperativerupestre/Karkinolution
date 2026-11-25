#include "karkinolution/networking/config_output.hpp"
#include "karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/networking/server_side/simulation/simulation.hpp"
#include "karkinolution/networking/world_state.hpp"
#include "karkinolution/testing/helpers/world.hpp"
#include "karkinolution/world/world_storage.hpp"

#include <array>
#include <atomic>
#include <gtest/gtest.h>
#include <latch>
#include <memory>
#include <thread>

TEST(WorldOwnership, RecordsTheConnectionThatAcquiredTheWorld) {
	Karkinolution::World::WorldStorage storage;
	ASSERT_TRUE(storage.add(Karkinolution::Testing::World::make_small_world()));
	const auto                                                    world_id = storage.get_all_world_ids().front();
	Karkinolution::Networking::ServerSide::Simulation::Simulation simulation{storage};
	Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter adapter{simulation, 17};
	std::optional<KShared::Functional::GiftLease<Karkinolution::World::World>>              lease;

	ASSERT_TRUE(adapter.config(world_id, lease).could_connect);
	ASSERT_TRUE(lease.has_value());

	const auto state = simulation.get_world_state(world_id);
	ASSERT_TRUE(state.has_value());
	ASSERT_TRUE(state->owner.has_value());
	EXPECT_EQ(state->owner.value(), 17);
}

TEST(WorldOwnership, ConcurrentConnectionsOnlyOneAcquiresTheWorld) {
	Karkinolution::World::WorldStorage storage;
	ASSERT_TRUE(storage.add(Karkinolution::Testing::World::make_small_world()));
	const auto                                                    world_id = storage.get_all_world_ids().front();
	Karkinolution::Networking::ServerSide::Simulation::Simulation simulation{storage};
	Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter first{simulation, 1};
	Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter second{simulation, 2};
	std::optional<KShared::Functional::GiftLease<Karkinolution::World::World>>              first_lease;
	std::optional<KShared::Functional::GiftLease<Karkinolution::World::World>>              second_lease;
	std::latch                                                                              start{2};
	Karkinolution::Networking::ConfigOutput                                                 first_result{};
	Karkinolution::Networking::ConfigOutput                                                 second_result{};

	std::jthread first_thread{[&] {
		start.count_down();
		start.wait();
		first_result = first.config(world_id, first_lease);
	}};
	std::jthread second_thread{[&] {
		start.count_down();
		start.wait();
		second_result = second.config(world_id, second_lease);
	}};

	first_thread.join();
	second_thread.join();

	EXPECT_NE(first_result.could_connect, second_result.could_connect);
	EXPECT_EQ(first_lease.has_value(), first_result.could_connect);
	EXPECT_EQ(second_lease.has_value(), second_result.could_connect);
}

TEST(WorldOwnership, ConcurrentCreateWorldOperationsConnectEachCreatorAtomically) {
	Karkinolution::World::WorldStorage                            storage;
	Karkinolution::Networking::ServerSide::Simulation::Simulation simulation{storage};
	constexpr std::size_t                                         connection_count = 8;
	std::array<Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter*,
			   connection_count>
		adapters{};
	std::array<std::unique_ptr<Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter>,
			   connection_count>
		adapter_storage;
	for (std::size_t index = 0; index < connection_count; ++index) {
		adapter_storage[index] =
			std::make_unique<Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter>(
				simulation,
				index + 1);
		adapters[index] = adapter_storage[index].get();
	}

	std::array<std::optional<KShared::Functional::GiftLease<Karkinolution::World::World>>, connection_count> leases;
	std::array<bool, connection_count>                                                                       results{};
	std::latch                start{connection_count};
	std::vector<std::jthread> threads;
	threads.reserve(connection_count);
	for (std::size_t index = 0; index < connection_count; ++index) {
		threads.emplace_back([&, index] {
			start.count_down();
			start.wait();
			results[index] = adapters[index]->create_world(Karkinolution::Testing::World::make_small_world_preset(),
														   true,
														   leases[index]);
		});
	}

	for (auto &thread : threads) {
		thread.join();
	}

	for (std::size_t index = 0; index < connection_count; ++index) {
		EXPECT_TRUE(results[index]);
		EXPECT_TRUE(leases[index].has_value());
	}
}

TEST(WorldOwnership, ConcurrentAcquisitionCannotStealWorldDuringCreateAndConnect) {
	Karkinolution::World::WorldStorage                                                      storage;
	Karkinolution::Networking::ServerSide::Simulation::Simulation                           simulation{storage};
	Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter creator{simulation, 1};
	constexpr std::size_t                                                                   contender_count = 7;
	std::array<std::unique_ptr<Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter>,
			   contender_count>
		contenders;
	for (std::size_t index = 0; index < contender_count; ++index) {
		contenders[index] =
			std::make_unique<Karkinolution::Networking::ServerSide::Connection::SimulationAdapter::SimulationAdapter>(
				simulation,
				index + 2);
	}

	std::optional<KShared::Functional::GiftLease<Karkinolution::World::World>> creator_lease;
	std::array<std::optional<KShared::Functional::GiftLease<Karkinolution::World::World>>, contender_count>
																		 contender_leases;
	std::array<Karkinolution::Networking::ConfigOutput, contender_count> contender_results{};
	std::latch                                                           start{contender_count + 1};
	std::atomic<Karkinolution::World::WorldId>                           created_world_id{0};
	std::atomic<bool>                                                    creator_result{false};
	std::jthread                                                         creator_thread{[&] {
        start.count_down();
        start.wait();
        creator_result.store(
            creator.create_world(Karkinolution::Testing::World::make_small_world_preset(), true, creator_lease));
    }};

	std::vector<std::jthread> contender_threads;
	contender_threads.reserve(contender_count);
	for (std::size_t index = 0; index < contender_count; ++index) {
		contender_threads.emplace_back([&, index] {
			start.count_down();
			start.wait();
			Karkinolution::World::WorldId world_id = 0;
			while (world_id == 0) {
				const auto ids = storage.get_all_world_ids();
				if (!ids.empty()) {
					world_id = ids.front();
				}
			}
			created_world_id.store(world_id);
			contender_results[index] = contenders[index]->config(world_id, contender_leases[index]);
		});
	}

	creator_thread.join();
	for (auto &thread : contender_threads) {
		thread.join();
	}

	EXPECT_TRUE(creator_result.load());
	ASSERT_TRUE(creator_lease.has_value());
	for (std::size_t index = 0; index < contender_count; ++index) {
		EXPECT_FALSE(contender_results[index].could_connect);
		EXPECT_FALSE(contender_leases[index].has_value());
	}
	EXPECT_EQ(simulation.get_world_state(created_world_id.load())->owner.value(), 1);
}
