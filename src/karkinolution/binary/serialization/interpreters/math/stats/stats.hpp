#pragma once

#include "karkinolution/binary/binary_validators.hpp"
#include "karkinolution/binary/deserialization/deserializer.hpp"
#include "karkinolution/binary/serialization/serializer.hpp"

#include <array>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/math/stats/concepts.hpp>
#include <karkinolution/math/stats/getter.hpp>

namespace StatsSRI {
	/*
	 * Protocol for LimitedValue, RuntimeLimitedValue, NormalizedValue, SignedNormalizedValue...
	 * and each variant of LimitedValue/RuntimeLimitedValue
	 *
	 *
	 * Protocol:
	 *
	 * [1 byte - numeric type]
	 * [8 bytes - value]
	 * [8 bytes - max]
	 * [8 bytes - min]
	 */

	inline constexpr std::size_t TypeBytes  = 1;
	inline constexpr std::size_t ValueBytes = 8;
	inline constexpr std::size_t MaxBytes   = 8;
	inline constexpr std::size_t MinBytes   = 8;

	inline constexpr std::size_t TO_GET_TYPE_OFFSET  = 0;
	inline constexpr std::size_t TO_GET_VALUE_OFFSET = TO_GET_TYPE_OFFSET + TypeBytes;
	inline constexpr std::size_t TO_GET_MAX_OFFSET   = TO_GET_VALUE_OFFSET + ValueBytes;
	inline constexpr std::size_t TO_GET_MIN_OFFSET   = TO_GET_MAX_OFFSET + MaxBytes;

	using StatBytes = std::array<std::byte, TypeBytes + ValueBytes + MaxBytes + MinBytes>;

	template <StatLike T> StatBytes serialize_generic_limited_value(const T &limited_value) {
		StatBytes bytes{};

		using ValueType = typename T::value_type;

		BinaryNLTValidator::validate_dynamic_type_exists<ValueType>();

		constexpr auto NTL_Type = BinaryNLTUtils::get<ValueType>();

		bytes[TO_GET_TYPE_OFFSET] = static_cast<std::byte>(static_cast<std::uint8_t>(NTL_Type));

		const auto stat = StatsGetter::get(limited_value);

		const auto serialized_value = Serializer::serialize(stat.value);
		const auto serialized_max   = Serializer::serialize(stat.max);
		const auto serialized_min   = Serializer::serialize(stat.min);

		Deserializer::append_bytes(bytes, serialized_value, TO_GET_VALUE_OFFSET);
		Deserializer::append_bytes(bytes, serialized_max, TO_GET_MAX_OFFSET);
		Deserializer::append_bytes(bytes, serialized_min, TO_GET_MIN_OFFSET);

		return bytes;
	}
} // namespace StatsSRI