#pragma once
#include <cstdint>
#include <karkinolution/utils/k_random.hpp>
#include <random>

inline uint64_t gen_id() {
	std::uniform_int_distribution<uint64_t> dist(std::numeric_limits<uint64_t>::min(),
												 std::numeric_limits<uint64_t>::max());
	return dist(gen);
}