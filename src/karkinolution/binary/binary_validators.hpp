#pragma once
#include "karkinolution/binary/core_aliases.hpp"

#include <cstdint>
#include <format>
#include <karkinolution/binary/protocol/aliases.hpp>
#include <karkinolution/binary/protocol/contract.hpp>
#include <karkinolution/core/error.hpp>
#include <magic_enum/magic_enum.hpp>
#include <string>

namespace Karkinolution {
	namespace Binary {
		namespace Protocol {
			namespace Validator {
				constexpr void validate_type(uint8_t value) {
					if (!magic_enum::enum_contains<BinaryTypes>(value)) {
						throw ByteError(std::format("Invalid frame type: {}", value));
					}
				}

				namespace Error {
					constexpr void validate_sub_type(uint8_t value) {
						if (!magic_enum::enum_contains<ErrorSubTypes>(value)) {
							throw ByteError(std::format("Invalid frame error sub type: {}", value));
						}
					}
				} // namespace Error

				namespace Request {
					constexpr void validate_sub_type(uint8_t value) {
						if (!magic_enum::enum_contains<RequestSubTypes>(value)) {
							throw ByteError(
								std::format("Invalid frame request sub type: {}", value));
						}
					}

					constexpr void validate_resource(uint8_t value) {
						if (!magic_enum::enum_contains<Resource>(value)) {
							throw ByteError(std::format("Invalid frame resource: {}", value));
						}
					}

				} // namespace Request

				namespace Response {
					constexpr void validate_sub_type(uint8_t value) {
						if (!magic_enum::enum_contains<ResponseSubTypes>(value)) {
							throw ByteError(std::format("Invalid frame sub type: {}", value));
						}
					}
				} // namespace Response

				constexpr void validate_sub_type(BinaryTypes type, uint8_t value) {
					switch (type) {
						case BinaryTypes::Request:
							Request::validate_sub_type(value);
							break;
						case BinaryTypes::Response:
							Response::validate_sub_type(value);
							break;
						case BinaryTypes::Error:
							Error::validate_sub_type(value);
							break;
						default:
							throw ByteError(
								std::format("Invalid type: {}", static_cast<uint8_t>(type)));
					}
				}

				namespace NumericType {
					template <typename T> constexpr void validate_dynamic_type_exists() {
						if (!(std::is_same_v<T, float> || std::is_same_v<T, double>
							  || std::is_same_v<T, uint8_t> || std::is_same_v<T, uint32_t>
							  || std::is_same_v<T, uint16_t> || std::is_same_v<T, uint64_t>)) {
							throw ByteError(
								"Type doesn't exist as dynamic numeric language (binary) type of "
								"karkinolution");
						}
					}
				} // namespace NumericType
			} // namespace Validator
		} // namespace Protocol
	} // namespace Binary
} // namespace Karkinolution

namespace BinaryValidator {} // namespace BinaryValidator
