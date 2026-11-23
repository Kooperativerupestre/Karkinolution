#include <forward_list>
#include <gtest/gtest.h>
#include <shared/functional/k_random.hpp>
#include <vector>

using namespace KShared::Functional;

TEST(KRandomChoice, ThrowsOnEmptyVector) {
	std::vector<int> empty_vec;
	EXPECT_THROW(KRandom::choice(empty_vec), std::invalid_argument);
}

TEST(KRandomChoice, ThrowsOnEmptyInitializerList) {
	std::initializer_list<int> empty_list = {};
	EXPECT_THROW(KRandom::choice(empty_list), std::invalid_argument);
}

TEST(KRandomChoice, ThrowsOnEmptyForwardRange) {
	std::forward_list<int> empty_flist;
	EXPECT_THROW(KRandom::choice(empty_flist), std::invalid_argument);
}

TEST(KRandomChoices, ValidSampling) {
	std::vector<int>    items   = {10, 20, 30};
	std::vector<double> weights = {1.0, 2.0, 3.0};

	auto result = KRandom::choices(items, weights, 5);
	EXPECT_EQ(result.size(), 5);
	for (int val : result) {
		EXPECT_TRUE(val == 10 || val == 20 || val == 30);
	}

	auto zero_k = KRandom::choices(items, weights, 0);
	EXPECT_TRUE(zero_k.empty());
}

TEST(KRandomChoices, ThrowsOnMismatchedSizes) {
	std::vector<int>    items   = {1, 2, 3};
	std::vector<double> weights = {1.0, 2.0};
	EXPECT_THROW(KRandom::choices(items, weights, 2), std::invalid_argument);
}

TEST(KRandomChoices, ThrowsOnEmptyItemsWithPositiveK) {
	std::vector<int>    items;
	std::vector<double> weights;
	EXPECT_THROW(KRandom::choices(items, weights, 2), std::invalid_argument);
}

TEST(KRandomChoices, ThrowsOnNegativeWeights) {
	std::vector<int>    items   = {1, 2};
	std::vector<double> weights = {1.0, -0.5};
	EXPECT_THROW(KRandom::choices(items, weights, 2), std::invalid_argument);
}

TEST(KRandomChoices, ThrowsOnZeroTotalWeight) {
	std::vector<int>    items   = {1, 2};
	std::vector<double> weights = {0.0, 0.0};
	EXPECT_THROW(KRandom::choices(items, weights, 2), std::invalid_argument);
}
