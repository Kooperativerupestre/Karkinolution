#include "shared/functional/unique_gift.hpp"

#include <atomic>
#include <barrier>
#include <gtest/gtest.h>
#include <thread>
#include <type_traits>
#include <vector>

using KShared::Functional::BadGiftAccess;
using KShared::Functional::BadOptionalReferenceAccessError;
using KShared::Functional::GiftLease;
using KShared::Functional::UniqueGift;

static_assert(!std::is_copy_constructible_v<UniqueGift<int>>);
static_assert(!std::is_copy_assignable_v<UniqueGift<int>>);
static_assert(!std::is_copy_constructible_v<GiftLease<int>>);
static_assert(!std::is_copy_assignable_v<GiftLease<int>>);

TEST(UniqueGiftTest, TakingAnAlreadyTakenGiftThrows) {
	UniqueGift<int> gift{42};
	const auto      lease = gift.take();

	EXPECT_TRUE(gift.was_taken());
	EXPECT_THROW(gift.take(), BadGiftAccess);
}

TEST(UniqueGiftTest, SevenConcurrentTakesAllowExactlyOneLease) {
	UniqueGift<int>          gift{42};
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
				auto lease = gift.take();
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
	EXPECT_FALSE(gift.was_taken());
}

TEST(UniqueGiftTest, DestroyingAGiftLeaseMakesTheValueAvailableAgain) {
	UniqueGift<int> gift{42};

	{
		auto lease  = gift.take();
		lease.get() = 84;
		EXPECT_TRUE(gift.was_taken());
	}

	EXPECT_FALSE(gift.was_taken());
	auto returned_lease = gift.take();
	EXPECT_EQ(returned_lease.get(), 84);
}

TEST(UniqueGiftTest, GiftLeaseCanBeGivenBackExplicitly) {
	UniqueGift<int> gift{42};
	auto            lease = gift.take();

	gift.give_back(lease);

	EXPECT_FALSE(gift.was_taken());
	EXPECT_THROW(lease.get(), BadOptionalReferenceAccessError);
}

TEST(UniqueGiftTest, GiftLeaseGetThrowsAfterTheValueWasGivenBack) {
	UniqueGift<int> gift{42};
	auto            lease = gift.take();

	gift.give_back(lease);

	EXPECT_THROW(static_cast<const GiftLease<int> &>(lease).get(), BadOptionalReferenceAccessError);
}
