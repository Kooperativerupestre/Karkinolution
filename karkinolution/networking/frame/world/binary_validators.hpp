#pragma once

#include "karkinolution/networking/frame/world/contract.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"

#include <cstdint>
#include <format>
#include <magic_enum/magic_enum.hpp>
#include <shared/binary/errors.hpp>
#include <string>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Validator {
				constexpr void validate_type(uint8_t value) {
					if (!magic_enum::enum_contains<Type>(value)) {
						throw KShared::Binary::ByteError(std::format("Invalid frame type: {}", value));
					}
				}

				namespace Error {
					constexpr void validate_sub_type(uint8_t value) {
						if (!magic_enum::enum_contains<ErrorSubTypes>(value)) {
							throw KShared::Binary::ByteError(std::format("Invalid frame error sub type: {}", value));
						}
					}
				} // namespace Error

				namespace Request {
					constexpr void validate_sub_type(uint8_t value) {
						if (!magic_enum::enum_contains<RequestSubTypes>(value)) {
							throw KShared::Binary::ByteError(std::format("Invalid frame request sub type: {}", value));
						}
					}

					constexpr void validate_resource(uint8_t value) {
						if (!magic_enum::enum_contains<Frames::World::Request::Resource>(value)) {
							throw KShared::Binary::ByteError(std::format("Invalid frame resource: {}", value));
						}
					}

				} // namespace Request

				namespace Response {
					constexpr void validate_sub_type(uint8_t value) {
						if (!magic_enum::enum_contains<ResponseSubTypes>(value)) {
							throw KShared::Binary::ByteError(std::format("Invalid frame sub type: {}", value));
						}
					}
				} // namespace Response

				constexpr void validate_sub_type(Type type, uint8_t value) {
					switch (type) {
						case Type::Request:
							Request::validate_sub_type(value);
							break;
						case Type::Response:
							Response::validate_sub_type(value);
							break;
						case Type::Error:
							Error::validate_sub_type(value);
							break;
					}
				}

				namespace NumericType {
					constexpr void validate(uint8_t value) {
						if (!magic_enum::enum_contains<Frames::World::NumericType>(value)) {
							throw KShared::Binary::ByteError(std::format("Invalid numeric type: {}", value));
						}
					}
				} // namespace NumericType
			} // namespace Validator
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
