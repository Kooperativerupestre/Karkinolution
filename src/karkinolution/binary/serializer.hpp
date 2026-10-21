#pragma once
#include <array>
#include <cstdint>
#include <karkinolution/binary/binary_supported_types.hpp>
#include <string>
#include <vector>

namespace Serializer {

	namespace Core {
		template <typename T>
		concept HasEightBytes = sizeof(T) == 8;

		template <typename T>
		concept HasOneByte = sizeof(T) == 1;

		template <typename T>
		concept HasTwoBytes = sizeof(T) == 2;

		template <typename T>
		concept HasFourBytes = sizeof(T) == 4;

		using EightBytes = std::array<std::byte, 8>;
		using FourBytes  = std::array<std::byte, 4>;
		using TwoBytes   = std::array<std::byte, 2>;

		template <HasEightBytes T> constexpr EightBytes convert_8_bytes(T value) {
			EightBytes bytes;

			bytes[0] = static_cast<std::byte>(value >> 56 & 0xFF);
			bytes[1] = static_cast<std::byte>(value >> 48 & 0xFF);
			bytes[2] = static_cast<std::byte>(value >> 40 & 0xFF);
			bytes[3] = static_cast<std::byte>(value >> 32 & 0xFF);
			bytes[4] = static_cast<std::byte>(value >> 24 & 0xFF);
			bytes[5] = static_cast<std::byte>(value >> 16 & 0xFF);
			bytes[6] = static_cast<std::byte>(value >> 8 & 0xFF);
			bytes[7] = static_cast<std::byte>(value & 0xFF);

			return bytes;
		}

		template <HasFourBytes T> constexpr FourBytes convert_4_bytes(T value) {
			FourBytes bytes;

			bytes[0] = static_cast<std::byte>(value >> 24 & 0xFF);
			bytes[1] = static_cast<std::byte>(value >> 16 & 0xFF);
			bytes[2] = static_cast<std::byte>(value >> 8 & 0xFF);
			bytes[3] = static_cast<std::byte>(value & 0xFF);

			return bytes;
		}

		template <HasTwoBytes T> constexpr TwoBytes convert_2_bytes(T value) {
			TwoBytes bytes;

			bytes[0] = static_cast<std::byte>(value >> 8 & 0xFF);
			bytes[1] = static_cast<std::byte>(value & 0xFF);

			return bytes;
		}

		template <HasOneByte T> constexpr std::byte convert_1_byte(T value) {
			return static_cast<std::byte>(value);
		}
	} // namespace Core

	Core::FourBytes  convert_uint32_t(std::uint32_t value);
	Core::TwoBytes   convert_uint16_t(std::uint16_t value);
	Core::EightBytes convert_double(double value);
	Core::FourBytes  convert_float(float value);
	Core::EightBytes convert_uint64_t(std::uint64_t value);
	std::byte        convert_uint8_t(std::uint8_t value);

	Core::FourBytes convert_int(int value);

	Core::FourBytes  convert_int32_t(std::int32_t value);
	Core::EightBytes convert_int64_t(std::int64_t value);
	Core::TwoBytes   convert_int16_t(std::int16_t value);
	std::byte        convert_int8_t(std::int8_t value);
	// 0x00 = false
	// 0x01 = true
	std::byte convert_bool(bool value);

	std::vector<std::byte> convert_string(const std::string &string);

	template <BinarySupportedType T> auto serialize(T value) {
		if constexpr (std::same_as<T, double>) {
			return convert_double(value);
		} else if constexpr (std::same_as<T, float>) {
			return convert_float(value);
		} else if constexpr (std::same_as<T, std::uint8_t>) {
			return convert_uint8_t(value);
		} else if constexpr (std::same_as<T, std::uint16_t>) {
			return convert_uint16_t(value);
		} else if constexpr (std::same_as<T, std::uint32_t>) {
			return convert_uint32_t(value);
		} else if constexpr (std::same_as<T, std::uint64_t>) {
			return convert_uint64_t(value);
		} else if constexpr (std::same_as<T, std::int8_t>) {
			return convert_int8_t(value);
		} else if constexpr (std::same_as<T, std::int16_t>) {
			return convert_int16_t(value);
		} else if constexpr (std::same_as<T, std::int32_t>) {
			return convert_int32_t(value);
		} else if constexpr (std::same_as<T, std::int64_t>) {
			return convert_int64_t(value);
		} else if constexpr (std::same_as<T, std::string>) {
			return convert_string(value);
		} else if constexpr (std::same_as<T, bool>) {
			return convert_bool(value);
		}
	}

} // namespace Serializer