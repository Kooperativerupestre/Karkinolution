#pragma once
#include <cstdint>
#include <variant>

/*

Protocol

size -> type -> sub type -> payload

[sizes](message_type_size.hpp)

The value of the field size means the number of bytes after the size field.
*/

inline constexpr std::size_t MESSAGE_SUB_TYPE_BYTES = sizeof(std::uint32_t); // bytes
inline constexpr std::size_t MESSAGE_TYPE_BYTES     = sizeof(std::uint8_t);  // bytes
inline constexpr std::size_t MESSAGE_SIZE_BYTES     = sizeof(std::uint32_t); // bytes

inline constexpr std::size_t MESSAGE_HEADER_BYTES =
	MESSAGE_SIZE_BYTES + MESSAGE_TYPE_BYTES + MESSAGE_SUB_TYPE_BYTES;


enum class BinaryTypes : uint8_t {
	Request  = 1,
	Error    = 2,
	Response = 3
};

namespace BinarySubTypes {
	enum class Request : uint32_t {
		GET_CREATURE = 1,
	};


	enum class Error : uint32_t {
		CREATURE_WAS_NOT_FOUND = 1
	};

	enum class Response : uint32_t {
		CREATURE
	};

	using CodeSubTypes = std::variant<Request, Error, Response>;

} // namespace BinarySubTypes
