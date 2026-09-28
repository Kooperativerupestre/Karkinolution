#pragma once

#include "karkinolution/binary/binary_error.hpp"
#include "karkinolution/binary/binary_supported_types.hpp"
#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/binary/type_string.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <format>
#include <karkinolution/core/error.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace Deserializer {

	namespace Core {

		using Generic8BytesType = std::uint64_t;
		using Generic4BytesType = std::uint32_t;
		using Generic2BytesType = std::uint16_t;
		using Generic1BytesType = std::byte;

		template <ByteRange T>
		constexpr Generic8BytesType deserialize_8_bytes(const T &bytes, std::size_t offset) {
			if (offset > bytes.size() || bytes.size() - offset < 8) {
				throw ByteError(BinaryErrorFactory::failed_conversion(
					std::format("the size of bytes ({}) is insufficient for 8 bytes at offset {}",
								bytes.size(),
								offset),
					std::format("{} (generic type for eight bytes)", nameof<Generic8BytesType>())));
			}

			return (std::to_integer<std::uint64_t>(bytes[offset]) << 56)
				| (std::to_integer<std::uint64_t>(bytes[offset + 1]) << 48)
				| (std::to_integer<std::uint64_t>(bytes[offset + 2]) << 40)
				| (std::to_integer<std::uint64_t>(bytes[offset + 3]) << 32)
				| (std::to_integer<std::uint64_t>(bytes[offset + 4]) << 24)
				| (std::to_integer<std::uint64_t>(bytes[offset + 5]) << 16)
				| (std::to_integer<std::uint64_t>(bytes[offset + 6]) << 8)
				| std::to_integer<std::uint64_t>(bytes[offset + 7]);
		}

		template <ByteRange T>
		constexpr Generic4BytesType deserialize_4_bytes(const T &bytes, std::size_t offset) {
			if (offset > bytes.size() || bytes.size() - offset < 4) {
				throw ByteError(BinaryErrorFactory::failed_conversion(
					std::format("the size of bytes ({}) is insufficient for 4 bytes at offset {}",
								bytes.size(),
								offset),
					std::format("{} (generic type for four bytes)", nameof<Generic4BytesType>())));
			}

			return (std::to_integer<std::uint32_t>(bytes[offset]) << 24)
				| (std::to_integer<std::uint32_t>(bytes[offset + 1]) << 16)
				| (std::to_integer<std::uint32_t>(bytes[offset + 2]) << 8)
				| std::to_integer<std::uint32_t>(bytes[offset + 3]);
		}

		template <ByteRange T>
		constexpr Generic2BytesType deserialize_2_bytes(const T &bytes, std::size_t offset) {
			if (offset > bytes.size() || bytes.size() - offset < 2) {
				throw ByteError(BinaryErrorFactory::failed_conversion(
					std::format("the size of bytes ({}) is insufficient for 2 bytes at offset {}",
								bytes.size(),
								offset),
					std::format("{} (generic type for two bytes)", nameof<Generic2BytesType>())));
			}

			return (std::to_integer<std::uint16_t>(bytes[offset]) << 8)
				| std::to_integer<std::uint16_t>(bytes[offset + 1]);
		}

		template <ByteRange T>
		constexpr Generic1BytesType deserialize_1_byte(const T &bytes, std::size_t offset) {
			if (offset >= bytes.size()) {
				throw ByteError(BinaryErrorFactory::failed_conversion(
					std::format("the size of bytes ({}) is insufficient for 1 byte at offset {}",
								bytes.size(),
								offset),
					std::format("{} (generic type for one byte)", nameof<Generic1BytesType>())));
			}

			return bytes[offset];
		}

	} // namespace Core

	namespace Types {

		class Bytes8Deserialized {
			private:

				std::uint64_t real_value;

			public:

				template <ByteRange T>
				constexpr Bytes8Deserialized(const T &bytes, std::size_t offset)
					: real_value(Core::deserialize_8_bytes(bytes, offset)) {}

				constexpr std::uint64_t as_uint64_t() const {
					return real_value;
				}

				constexpr std::int64_t as_int64_t() const {
					return std::bit_cast<std::int64_t>(real_value);
				}

				constexpr double as_double() const {
					return std::bit_cast<double>(real_value);
				}
		};

		class Bytes4Deserialized {
			private:

				std::uint32_t real_value;

			public:

				template <ByteRange T>
				constexpr Bytes4Deserialized(const T &bytes, std::size_t offset)
					: real_value(Core::deserialize_4_bytes(bytes, offset)) {}

				constexpr std::uint32_t as_uint32_t() const {
					return real_value;
				}

				constexpr std::int32_t as_int32_t() const {
					return std::bit_cast<std::int32_t>(real_value);
				}

				constexpr int as_int() const {
					return std::bit_cast<int>(real_value);
				}

				constexpr float as_float() const {
					return std::bit_cast<float>(real_value);
				}
		};

		class Bytes2Deserialized {
			private:

				std::uint16_t real_value;

			public:

				template <ByteRange T>
				constexpr Bytes2Deserialized(const T &bytes, std::size_t offset)
					: real_value(Core::deserialize_2_bytes(bytes, offset)) {}

				constexpr std::uint16_t as_uint16_t() const {
					return real_value;
				}

				constexpr std::int16_t as_int16_t() const {
					return std::bit_cast<std::int16_t>(real_value);
				}
		};

		class Bytes1Deserialized {
			private:

				std::byte real_value;

			public:

				template <ByteRange T>
				constexpr Bytes1Deserialized(const T &bytes, std::size_t offset)
					: real_value(Core::deserialize_1_byte(bytes, offset)) {}

				constexpr std::byte as_byte() const {
					return real_value;
				}

				constexpr std::uint8_t as_uint8_t() const {
					return std::to_integer<std::uint8_t>(real_value);
				}
		};

	} // namespace Types

	template <ByteRange T>
	constexpr std::uint64_t read_uint64_t(const T &bytes, std::size_t offset) {
		return Types::Bytes8Deserialized(bytes, offset).as_uint64_t();
	}

	template <ByteRange T> constexpr std::int64_t read_int64_t(const T &bytes, std::size_t offset) {
		return Types::Bytes8Deserialized(bytes, offset).as_int64_t();
	}

	template <ByteRange T> constexpr double read_double(const T &bytes, std::size_t offset) {
		return Types::Bytes8Deserialized(bytes, offset).as_double();
	}

	template <ByteRange T>
	constexpr std::uint32_t read_uint32_t(const T &bytes, std::size_t offset) {
		return Types::Bytes4Deserialized(bytes, offset).as_uint32_t();
	}

	template <ByteRange T> constexpr std::int32_t read_int32_t(const T &bytes, std::size_t offset) {
		return Types::Bytes4Deserialized(bytes, offset).as_int32_t();
	}

	template <ByteRange T> constexpr float read_float(const T &bytes, std::size_t offset) {
		return Types::Bytes4Deserialized(bytes, offset).as_float();
	}

	template <ByteRange T>
	constexpr std::uint16_t read_uint16_t(const T &bytes, std::size_t offset) {
		return Types::Bytes2Deserialized(bytes, offset).as_uint16_t();
	}

	template <ByteRange T> constexpr std::int16_t read_int16_t(const T &bytes, std::size_t offset) {
		return Types::Bytes2Deserialized(bytes, offset).as_int16_t();
	}

	template <ByteRange T> constexpr std::uint8_t read_uint8_t(const T &bytes, std::size_t offset) {
		return Types::Bytes1Deserialized(bytes, offset).as_uint8_t();
	}

	template <ByteRange T> constexpr int read_int(const T &bytes, std::size_t offset) {
		return Types::Bytes4Deserialized(bytes, offset).as_int();
	}

	template <ByteRange T>
	constexpr std::string read_string(const T &bytes, std::size_t offset, std::size_t length) {
		if (offset > bytes.size() || bytes.size() - offset < length) {
			throw ByteError(BinaryErrorFactory::failed_conversion(
				std::format(
					"the size of bytes ({}) is insufficient for string of length {} at offset {}",
					bytes.size(),
					length,
					offset),
				"string"));
		}

		std::string result;
		result.reserve(length);

		for (std::size_t i = 0; i < length; ++i) {
			result.push_back(std::to_integer<char>(bytes[offset + i]));
		}

		return result;
	}

	template <BinarySupportedType T, ByteRange U>
	constexpr T deserialize(const U &bytes, std::size_t offset) {
		if constexpr (std::same_as<T, double>) {
			return read_double(bytes, offset);
		} else if constexpr (std::same_as<T, float>) {
			return read_float(bytes, offset);
		} else if constexpr (std::same_as<T, std::uint8_t>) {
			return read_uint8_t(bytes, offset);
		} else if constexpr (std::same_as<T, std::uint16_t>) {
			return read_uint16_t(bytes, offset);
		} else if constexpr (std::same_as<T, std::uint32_t>) {
			return read_uint32_t(bytes, offset);
		} else if constexpr (std::same_as<T, std::uint64_t>) {
			return read_uint64_t(bytes, offset);
		} else if constexpr (std::same_as<T, int>) {
			return read_int(bytes, offset);
		} else if constexpr (std::same_as<T, std::string>) {
			throw ByteError(BinaryErrorFactory::type_is_not_supported<std::string>(
				"deserialize<std::string> requires a string length"));
		}
		throw ByteError(BinaryErrorFactory::type_is_not_supported<T>("generic deserialize"));
	}

	template <ByteRange T> void append_bytes(std::vector<std::byte> &bytes, const T &value) {
		for (const auto &byte : value) {
			bytes.push_back(byte);
		}
	}

	inline void append_bytes(std::vector<std::byte> &bytes, std::byte value) {
		bytes.push_back(value);
	}

	template <ByteRange T> void append_bytes(std::deque<std::byte> &bytes, const T &value) {
		for (const auto &byte : value) {
			bytes.push_back(byte);
		}
	}

	template <ByteRange T, size_t Size>
	void append_bytes(std::array<std::byte, Size> &bytes, const T &value, size_t offset) {
		for (size_t i = 0; i < value.size(); i++) {
			bytes[offset + i] = value[i];
		}
	}

	template <ByteRange T>
	void append_bytes(std::vector<std::byte> &bytes, const T &value, std::size_t offset) {
		for (std::size_t i = 0; i < value.size(); i++) {
			bytes[offset + i] = value[i];
		}
	}

	inline void append_bytes(std::deque<std::byte> &bytes, std::byte value) {
		bytes.push_back(value);
	}

	template <std::size_t Size>
	void append_bytes(std::array<std::byte, Size> &bytes, std::byte value, std::size_t offset) {
		bytes[offset] = value;
	}
} // namespace Deserializer
