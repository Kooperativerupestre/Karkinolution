#pragma once
#include <cstdint>
#include <karkinolution/utils/k_random.hpp>
#include <random>

using BaseIdType = int64_t;

// respects the invariant: 1 < id < max(type)

inline BaseIdType gen_id() {
	std::uniform_int_distribution<BaseIdType> dist(1, std::numeric_limits<BaseIdType>::max());
	return dist(gen);
}
