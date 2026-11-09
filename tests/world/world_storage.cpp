#include "karkinolution/test/utils.hpp"

#include <atomic>
#include <barrier>
#include <gtest/gtest.h>
#include <karkinolution/world/world_storage.hpp>
#include <thread>
#include <vector>

using Karkinolution::World::EntryState;
using Karkinolution::World::WorldStorage;
using KShared::Functional::BadGiftAccess;

TEST(WorldStorageTest, ConcurrentTakesAllowOnlyOneLease) {
	WorldStorage storage;
	auto         world    = TestUtils::make_small_world();
	const auto   world_id = world.get_id();
	ASSERT_TRUE(storage.add(std::move(world)));

	std::barrier             start_barrier{7};
	std::barrier             finished_barrier{7};
	std::atomic<int>         successful_takes{0};
	std::atomic<int>         failed_takes{0};
	std::vector<std::thread> threads;
	threads.reserve(7);

	for (int index = 0; index < 7; ++index) {
		threads.emplace_back([&] {
			start_barrier.arrive_and_wait();

			try {
				auto lease = storage.take(world_id);
				if (!lease.has_value()) {
					failed_takes.fetch_add(1);
					finished_barrier.arrive_and_wait();
					return;
				}
				successful_takes.fetch_add(1);
				finished_barrier.arrive_and_wait();
			} catch (const BadGiftAccess &) {
				failed_takes.fetch_add(1);
				finished_barrier.arrive_and_wait();
			}
		});
	}

	for (auto &thread : threads) {
		thread.join();
	}

	EXPECT_EQ(successful_takes.load(), 1);
	EXPECT_EQ(failed_takes.load(), 6);
	EXPECT_EQ(storage.get_state(world_id), EntryState::NOT_TAKEN);
}

TEST(WorldStorageTest, ConcurrentStateReadsRemainConsistentWhileWorldIsTaken) {
	WorldStorage storage;
	auto         world    = TestUtils::make_small_world();
	const auto   world_id = world.get_id();
	ASSERT_TRUE(storage.add(std::move(world)));

	auto lease = storage.take(world_id);
	ASSERT_TRUE(lease.has_value());
	std::barrier             start_barrier{7};
	std::atomic<int>         invalid_observations{0};
	std::vector<std::thread> threads;
	threads.reserve(7);

	for (int index = 0; index < 7; ++index) {
		threads.emplace_back([&] {
			start_barrier.arrive_and_wait();
			if (!storage.has(world_id) || storage.get_state(world_id) != EntryState::TAKEN) {
				invalid_observations.fetch_add(1);
			}
		});
	}

	for (auto &thread : threads) {
		thread.join();
	}

	EXPECT_EQ(invalid_observations.load(), 0);
}
