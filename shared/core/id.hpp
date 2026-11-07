#pragma once
#include <cstdint>
#include <random>
#include <shared/functional/k_random.hpp>

namespace KShared {
	namespace Core {
		namespace Id {
			using BaseIdType = int64_t;

			// respects the invariant: 1 < id < max(type)

			inline BaseIdType gen_id() {
				std::uniform_int_distribution<BaseIdType> dist(1, std::numeric_limits<BaseIdType>::max());
				return dist(KShared::Functional::KRandom::gen);
			}
		} // namespace Id
	} // namespace Core
} // namespace KShared
