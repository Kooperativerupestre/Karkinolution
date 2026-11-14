#pragma once

#include "shared/binary/deserializer.hpp"
#include "shared/binary/fields.hpp"
#include "shared/binary/interpreters/models/stats.hpp"
#include "shared/binary/numeric_type.hpp"
#include "shared/binary/numeric_types.hpp"
#include "shared/binary/serializer.hpp"
#include "shared/math/stats/compile_values.hpp"

#include <array>
#include <shared/binary/byte_utils.hpp>
#include <shared/binary/interpreters/nlt.hpp>
#include <shared/math/stats/utils.hpp>
#include <vector>

namespace KShared {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Stats {
					/*
					 * Protocol for LimitedValue, RuntimeLimitedValue, NormalizedValue,
					 * SignedNormalizedValue... and each variant of LimitedValue/RuntimeLimitedValue
					 *
					 *
					 * Protocol:
					 *
					 * [1 byte - numeric type]
					 * [8 bytes - value]
					 * [8 bytes - max]
					 * [8 bytes - min]
					 */

					namespace Layout {
						using Type  = KShared::Binary::Field<0, 1>;
						using Value = KShared::Binary::Next<Type, 8>;
						using Max   = KShared::Binary::Next<Value, 8>;
						using Min   = KShared::Binary::Next<Max, 8>;

						inline constexpr std::size_t TOTAL_SIZE = Min::END;
					} // namespace Layout

					namespace BytesArray {
						using Stat = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArray

					template <KShared::Math::LV::StatLike T>
					BytesArray::Stat serialize_limited_value(const T &limited_value) {
						BytesArray::Stat bytes{};

						using ValueType = typename T::value_type;

						KShared::Binary::NumericTypes::validate<ValueType>();


						constexpr auto NTL_Type = KShared::Binary::NumericTypes::get<ValueType>();

						bytes[Layout::Type::OFFSET] = NLTSRI::serialize_nlt_type(NTL_Type);

						const auto stat = KShared::Math::LV::get(limited_value);

						const auto serialized_value = KShared::Binary::Serializer::serialize(stat.value);
						const auto serialized_max   = KShared::Binary::Serializer::serialize(stat.max);
						const auto serialized_min   = KShared::Binary::Serializer::serialize(stat.min);

						KShared::Binary::Utils::append_bytes(bytes, serialized_value, Layout::Value::OFFSET);
						KShared::Binary::Utils::append_bytes(bytes, serialized_max, Layout::Max::OFFSET);
						KShared::Binary::Utils::append_bytes(bytes, serialized_min, Layout::Min::OFFSET);

						return bytes;
					}

					template <typename T>
					BytesArray::Stat serialize_normalized_value(const KShared::Math::LV::CT::NormalizedValue<T> value) {
						BytesArray::Stat bytes{};

						KShared::Binary::NumericTypes::validate<T>();
						constexpr auto NLT_Type = KShared::Binary::NumericTypes::get<T>();

						bytes[Layout::Type::OFFSET] = NLTSRI::serialize_nlt_type(NLT_Type);
						const auto serialized_value = KShared::Binary::Serializer::serialize(value.value());
						const auto serialize_max    = KShared::Binary::Serializer::serialize(T(1));
						const auto serialized_min   = KShared::Binary::Serializer::serialize(T(0));

						KShared::Binary::Utils::append_bytes(bytes, serialized_value, Layout::Value::OFFSET);
						KShared::Binary::Utils::append_bytes(bytes, serialize_max, Layout::Max::OFFSET);
						KShared::Binary::Utils::append_bytes(bytes, serialized_min, Layout::Min::OFFSET);

						return bytes;
					}

					template <typename T>
					BytesArray::Stat
					serialize_signed_normalized_value(const KShared::Math::LV::CT::SignedNormalizedValue<T> value) {
						BytesArray::Stat bytes{};

						KShared::Binary::NumericTypes::validate<T>();
						constexpr auto NLT_Type = KShared::Binary::NumericTypes::get<T>();

						bytes[Layout::Type::OFFSET] = NLTSRI::serialize_nlt_type(NLT_Type);
						const auto serialized_value = KShared::Binary::Serializer::serialize(value.value());
						const auto serialize_max    = KShared::Binary::Serializer::serialize(T(1));
						const auto serialized_min   = KShared::Binary::Serializer::serialize(T(-1));

						KShared::Binary::Utils::append_bytes(bytes, serialized_value, Layout::Value::OFFSET);
						KShared::Binary::Utils::append_bytes(bytes, serialize_max, Layout::Max::OFFSET);
						KShared::Binary::Utils::append_bytes(bytes, serialized_min, Layout::Min::OFFSET);
						return bytes;
					}
				} // namespace Stats
			} // namespace SRI

			namespace DSI {
				namespace Stats {

					inline KShared::Binary::NumericType get_type(const std::vector<std::byte> &payload,
																 std::size_t                   offset) {
						return static_cast<KShared::Binary::NumericType>(
							KShared::Binary::Deserializer::read_uint8_t(payload,
																		offset + SRI::Stats::Layout::Type::OFFSET));
					}

					template <typename T> T get_value(const std::vector<std::byte> &payload, std::size_t offset) {
						return KShared::Binary::Deserializer::deserialize<T>(payload,
																			 offset
																				 + SRI::Stats::Layout::Value::OFFSET);
					}

					template <typename T> T get_max(const std::vector<std::byte> &payload, std::size_t offset) {
						return KShared::Binary::Deserializer::deserialize<T>(payload,
																			 offset + SRI::Stats::Layout::Max::OFFSET);
					}

					template <typename T> T get_min(const std::vector<std::byte> &payload, std::size_t offset) {
						return KShared::Binary::Deserializer::deserialize<T>(payload,
																			 offset + SRI::Stats::Layout::Min::OFFSET);
					}

					template <typename T>
					Interpreters::Models::Stats::StatValue<T> get_stats(const std::vector<std::byte> &payload,
																		std::size_t                   offset) {
						return {.value = get_value<T>(payload, offset),
								.min   = get_min<T>(payload, offset),
								.max   = get_max<T>(payload, offset)};
					}
				} // namespace Stats
			} // namespace DSI
		} // namespace Interpreters
	} // namespace Binary
} // namespace KShared

namespace StatsSRI = KShared::Binary::Interpreters::SRI::Stats;
namespace StatsDSI = KShared::Binary::Interpreters::DSI::Stats;
