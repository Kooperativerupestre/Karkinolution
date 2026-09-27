#pragma once
#include <karkinolution/math/stats/concepts.hpp>

namespace StatsGetter {

	template <typename T> struct GenericStat {
			T value, min, max;
	};

	template <StatLike T> auto get(const T &stat) {
		using Type = typename T::value_type;
		GenericStat<Type> result;

		result.value = stat.value();

		if constexpr (LimitedValueLike<T>) {
			result.max = stat.max;
			result.min = stat.min;
		} else {
			result.max = stat.max();
			result.min = stat.min();
		}

		return result;
	}

} // namespace StatsGetter