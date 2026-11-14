#include "karkinolution/networking/config_output.hpp"
#include "karkinolution/networking/server_side/connection/simulation_adapter/simulation_adapter.hpp"
#include "karkinolution/networking/server_side/simulation/simulation.hpp"
#include "karkinolution/networking/world_state.hpp"
#include "karkinolution/world/world_storage.hpp"

#include <gtest/gtest.h>
#include <latch>
#include <thread>

namespace {
	Karkinolution::World::World make_world() {
		return Karkinolution::World::World{Karkinolution::World::WorldPreset{
			.seed    = 1,
			.size    = {.width = KShared::Math::Units::Width{100.0}, .height = KShared::Math::Units::Height{100.0}},
			.epsilon = 5.0f}};
	}
} // namespace

TEST(WorldOwnership, RecordsTheConnectionThatAcquiredTheWorld) {
	Karkinolution::World::WorldStorage storage;
	ASSERT_TRUE(storage.add(make_world()));
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
	ASSERT_TRUE(storage.add(make_world()));
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
