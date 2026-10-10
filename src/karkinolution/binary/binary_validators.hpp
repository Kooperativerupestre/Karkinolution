#pragma once
#include "karkinolution/binary/core_aliases.hpp"

#include <cstdint>
#include <format>
#include <karkinolution/binary/message_type_size.hpp>
#include <karkinolution/core/error.hpp>
#include <magic_enum/magic_enum.hpp>
#include <string>

namespace BinaryValidator {
	constexpr void validate_type(uint8_t value) {
		if (!magic_enum::enum_contains(BinaryTypes(value))) {
			throw ByteError(std::format("Invalid frame type: {}", value));
		}
	}

	namespace Error {
		constexpr void validate_sub_type(uint8_t value) {
			if (!magic_enum::enum_contains(ErrorSubTypes(value))) {
				throw ByteError(std::format("Invalid frame error sub type: {}", value));
			}
		}
	} // namespace Error

	namespace Request {
		constexpr void validate_sub_type(uint8_t value) {
			if (!magic_enum::enum_contains(RequestSubTypes(value))) {
				throw ByteError(std::format("Invalid frame request sub type: {}", value));
			}
		}

		constexpr void validate_resource(Frames::Types::RequestResourceEnumType value) {
			if (!magic_enum::enum_contains(RequestResource(value))) {
				throw ByteError(std::format("Invalid frame resource: {}", value));
			}
		}

		constexpr void validate_request_sub_type_and_resource(uint8_t sub_type, uint8_t resource) {
			validate_sub_type(sub_type);
			if (sub_type == static_cast<uint8_t>(Frames::Types::BinarySubTypes::Request::SEE)) {
				if (resource != static_cast<uint8_t>(Frames::Types::RequestResource::CREATURE)) {
					throw ByteError(std::format("Invalid request sub type {} with resource {}",
												sub_type,
												resource));
				}
			}
		}
	} // namespace Request

	namespace Response {
		constexpr void validate_sub_type(uint8_t value) {
			if (!magic_enum::enum_contains(ResponseSubTypes(value))) {
				throw ByteError(std::format("Invalid frame sub type: {}", value));
			}
		}
	} // namespace Response

	constexpr void validate_sub_type(Frames::Types::BinaryTypes type, uint8_t value) {
		if (type == Frames::Types::BinaryTypes::Request) {
			Request::validate_sub_type(value);
		} else if (type == Frames::Types::BinaryTypes::Response) {
			Response::validate_sub_type(value);
		} else if (type == Frames::Types::BinaryTypes::Error) {
			Error::validate_sub_type(value);
		} else {
			throw ByteError(std::format("Invalid type: {}", static_cast<uint8_t>(type)));
		}
	}

	namespace BinaryNLTValidator {
		template <typename T> constexpr void validate_dynamic_type_exists() {
			if (!(std::is_same<T, float>() || std::is_same<T, double>()
				  || std::is_same<T, uint8_t>() || std::is_same<T, uint32_t>()
				  || std::is_same<T, uint16_t>() || std::is_same<T, uint64_t>())) {
				throw ByteError("Type doesn't exists as dynamic numeric language (binary) type of "
								"karkinolution");
			}
		}
	} // namespace BinaryNLTValidator
} // namespace BinaryValidator
