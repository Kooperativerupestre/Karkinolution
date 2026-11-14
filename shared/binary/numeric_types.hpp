#pragma once

#include "shared/binary/errors.hpp"
#include "shared/binary/numeric_type.hpp"

#include <type_traits>

namespace KShared::Binary::NumericTypes {

	template <typename T> constexpr NumericType get() {
		if constexpr (std::is_same_v<T, float>) {
			return NumericType::FLOAT;
		} else if constexpr (std::is_same_v<T, double>) {
			return NumericType::DOUBLE;
		} else if constexpr (std::is_same_v<T, std::uint8_t>) {
			return NumericType::UINT8;
		} else if constexpr (std::is_same_v<T, std::uint32_t>) {
			return NumericType::UINT32;
		} else if constexpr (std::is_same_v<T, std::uint64_t>) {
			return NumericType::UINT64;
		} else if constexpr (std::is_same_v<T, int>) {
			return NumericType::INT;
		} else {
			static_assert(std::is_same_v<T, void>, "Type is not a supported numeric binary type");
		}
	}

	template <typename T> constexpr void validate() {
		if constexpr (!(std::is_same_v<T, float> || std::is_same_v<T, double> || std::is_same_v<T, std::uint8_t>
						|| std::is_same_v<T, std::uint32_t> || std::is_same_v<T, std::uint16_t>
						|| std::is_same_v<T, std::uint64_t> || std::is_same_v<T, int>)) {
			throw ByteError("Type does not exist as a dynamic numeric binary type");
		}
	}

} // namespace KShared::Binary::NumericTypes
