#pragma once
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/deserialization/interpreters/math/stats/models.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/binary/serialization/interpreters/math/stats/stats.hpp>
#include <vector>

namespace StatsDSI {
	inline BinaryNLT get_type(const std::vector<std::byte> &payload,
	                          std::size_t                  offset = StatsSRI::TO_GET_TYPE_OFFSET) {
		return static_cast<BinaryNLT>(Deserializer::read_uint8_t(payload, offset));
	}

	template <typename T> T get_value(const std::vector<std::byte> &payload, std::size_t offset) {
		return Deserializer::deserialize<T>(payload, offset);
	}

	template <typename T> T get_max(const std::vector<std::byte> &payload) {
		return get_value<T>(payload, StatsSRI::TO_GET_MAX_OFFSET);
	}

	template <typename T> T get_min(const std::vector<std::byte> &payload) {
		return get_value<T>(payload, StatsSRI::TO_GET_MIN_OFFSET);
	}

	template <typename T>
	DeserializedStatValue<T> get_stats(const std::vector<std::byte> &payload) {
		return {.value = get_value<T>(payload, StatsSRI::TO_GET_VALUE_OFFSET),
				.min   = get_min<T>(payload),
				.max   = get_max<T>(payload)};
	}
} // namespace StatsDSI