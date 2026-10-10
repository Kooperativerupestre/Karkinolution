#pragma once
#include "godot_cpp/variant/utility_functions.hpp"

#include <concepts>
#include <cstdint>

namespace KarkinolutionGodot {
	template <std::integral T> int64_t to_int64(T value) {
		if constexpr (std::is_signed_v<T> || sizeof(T) < sizeof(uint64_t)) {
			return static_cast<int64_t>(value);
		} else {
			godot::UtilityFunctions::push_error("Value exceeds int64_t representation");
			return std::numeric_limits<int64_t>().max();
		}
		return static_cast<int64_t>(value);
	}
} // namespace KarkinolutionGodot