#pragma once
#include <karkinolution/binary/deserialization/deserializer.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/binary/serialization/interpreters/math/stats/alias.hpp>
#include <karkinolution/binary/serialization/interpreters/math/stats/stats.hpp>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Models {
			namespace Stats {
				template <typename T> struct StatValue {
						T value, min, max;
				};
			} // namespace Stats
		} // namespace Models
	} // namespace Binary
} // namespace Karkinolution

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace DSI {
				namespace Stats {

					inline Frames::Types::BinaryNLT get_type(const std::vector<std::byte> &payload,
															 std::size_t                   offset) {
						return static_cast<Frames::Types::BinaryNLT>(
							Deserializer::read_uint8_t(payload,
													   offset + StatsSRI::Layout::Type::OFFSET));
					}

					template <typename T>
					T get_value(const std::vector<std::byte> &payload, std::size_t offset) {
						return Deserializer::deserialize<T>(payload,
															offset
																+ StatsSRI::Layout::Value::OFFSET);
					}

					template <typename T>
					T get_max(const std::vector<std::byte> &payload, std::size_t offset) {
						return Deserializer::deserialize<T>(payload,
															offset + StatsSRI::Layout::Max::OFFSET);
					}

					template <typename T>
					T get_min(const std::vector<std::byte> &payload, std::size_t offset) {
						return Deserializer::deserialize<T>(payload,
															offset + StatsSRI::Layout::Min::OFFSET);
					}

					template <typename T>
					Models::Stats::StatValue<T> get_stats(const std::vector<std::byte> &payload,
														  std::size_t                   offset) {
						return {.value = get_value<T>(payload, offset),
								.min   = get_min<T>(payload, offset),
								.max   = get_max<T>(payload, offset)};
					}
				} // namespace Stats
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution
