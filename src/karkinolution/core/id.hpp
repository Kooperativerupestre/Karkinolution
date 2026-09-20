#pragma once
#include <cstdint>
#include <karkinolution/utils/k_random.hpp>
#include <random>

using BaseIdType = uint64_t;

inline uint64_t gen_id() {
	std::uniform_int_distribution<uint64_t> dist(std::numeric_limits<BaseIdType>::min(),
												 std::numeric_limits<BaseIdType>::max());
	return dist(gen);
}
