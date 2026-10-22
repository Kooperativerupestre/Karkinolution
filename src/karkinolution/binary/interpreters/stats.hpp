#pragma once

#include "karkinolution/binary/binary_validators.hpp"
#include "karkinolution/binary/deserializer.hpp"
#include "karkinolution/binary/fields.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/binary/serializer.hpp"
#include "karkinolution/math/stats/compile_values.hpp"

#include <array>
#include <karkinolution/binary/byte_utils.hpp>
#include <karkinolution/binary/interpreters/nlt.hpp>
#include <karkinolution/math/stats/concepts.hpp>
#include <karkinolution/math/stats/getter.hpp>
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
						using Type  = Field<0, 1>;
						using Value = Next<Type, 8>;
						using Max   = Next<Value, 8>;
						using Min   = Next<Max, 8>;

						inline constexpr std::size_t TOTAL_SIZE = Min::END;
					} // namespace Layout

					namespace BytesArray {
						using Stat = std::array<std::byte, Layout::TOTAL_SIZE>;
					} // namespace BytesArray

					template <StatLike T>
					BytesArray::Stat serialize_limited_value(const T &limited_value) {
						BytesArray::Stat bytes{};

						using ValueType = typename T::value_type;

						BinaryValidator::BinaryNLTValidator::validate_dynamic_type_exists<
							ValueType>();

						constexpr auto NTL_Type = BinaryNLTUtils::get<ValueType>();

						bytes[Layout::Type::OFFSET] = NLTSRI::serialize_nlt_type(NTL_Type);

						const auto stat = StatsGetter::get(limited_value);

						const auto serialized_value = Serializer::serialize(stat.value);
						const auto serialized_max   = Serializer::serialize(stat.max);
						const auto serialized_min   = Serializer::serialize(stat.min);

						BinaryUtils::append_bytes(bytes, serialized_value, Layout::Value::OFFSET);
						BinaryUtils::append_bytes(bytes, serialized_max, Layout::Max::OFFSET);
						BinaryUtils::append_bytes(bytes, serialized_min, Layout::Min::OFFSET);

						return bytes;
					}

					template <typename T>
					BytesArray::Stat serialize_normalized_value(const NormalizedValue<T> value) {
						BytesArray::Stat bytes{};

						BinaryValidator::BinaryNLTValidator::validate_dynamic_type_exists<T>();
						constexpr auto NLT_Type = BinaryNLTUtils::get<T>();

						bytes[Layout::Type::OFFSET] = NLTSRI::serialize_nlt_type(NLT_Type);
						const auto serialized_value = Serializer::serialize(value.value());
						const auto serialize_max    = Serializer::serialize(T(1));
						const auto serialized_min   = Serializer::serialize(T(0));

						BinaryUtils::append_bytes(bytes, serialized_value, Layout::Value::OFFSET);
						BinaryUtils::append_bytes(bytes, serialize_max, Layout::Max::OFFSET);
						BinaryUtils::append_bytes(bytes, serialized_min, Layout::Min::OFFSET);

						return bytes;
					}

					template <typename T>
					BytesArray::Stat
					serialize_signed_normalized_value(const SignedNormalizedValue<T> value) {
						BytesArray::Stat bytes{};

						BinaryValidator::BinaryNLTValidator::validate_dynamic_type_exists<T>();
						constexpr auto NLT_Type = BinaryNLTUtils::get<T>();

						bytes[Layout::Type::OFFSET] = NLTSRI::serialize_nlt_type(NLT_Type);
						const auto serialized_value = Serializer::serialize(value.value());
						const auto serialize_max    = Serializer::serialize(T(1));
						const auto serialized_min   = Serializer::serialize(T(-1));

						BinaryUtils::append_bytes(bytes, serialized_value, Layout::Value::OFFSET);
						BinaryUtils::append_bytes(bytes, serialize_max, Layout::Max::OFFSET);
						BinaryUtils::append_bytes(bytes, serialized_min, Layout::Min::OFFSET);
						return bytes;
					}
				} // namespace Stats
			} // namespace SRI

			namespace DSI {
				namespace Stats {

					inline Protocol::Types::BinaryNLT
					get_type(const std::vector<std::byte> &payload, std::size_t offset) {
						return static_cast<Protocol::Types::BinaryNLT>(
							Deserializer::read_uint8_t(payload,
													   offset + SRI::Stats::Layout::Type::OFFSET));
					}

					template <typename T>
					T get_value(const std::vector<std::byte> &payload, std::size_t offset) {
						return Deserializer::deserialize<T>(
							payload,
							offset + SRI::Stats::Layout::Value::OFFSET);
					}

					template <typename T>
					T get_max(const std::vector<std::byte> &payload, std::size_t offset) {
						return Deserializer::deserialize<T>(payload,
															offset
																+ SRI::Stats::Layout::Max::OFFSET);
					}

					template <typename T>
					T get_min(const std::vector<std::byte> &payload, std::size_t offset) {
						return Deserializer::deserialize<T>(payload,
															offset
																+ SRI::Stats::Layout::Min::OFFSET);
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

namespace StatsSRI = Karkinolution::Binary::Interpreters::SRI::Stats;
namespace StatsDSI = Karkinolution::Binary::Interpreters::DSI::Stats;
