#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace Serializer {

	namespace Core {
		template <typename T>
		concept HasEightBytes = sizeof(T) == 8;

		template <typename T>
		concept HasOneBytes = sizeof(T) == 1;

		template <typename T>
		concept HasTwoBytes = sizeof(T) == 2;

		template <typename T>
		concept HasFourBytes = sizeof(T) == 4;

		using EightBytes = std::array<std::byte, 8>;
		using FourBytes  = std::array<std::byte, 4>;
		using TwoBytes   = std::array<std::byte, 2>;

		template <HasEightBytes T> EightBytes constexpr convert_8_bytes(T value) {
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

		template <HasFourBytes T> FourBytes constexpr convert_4_bytes(T value) {
			FourBytes bytes;

			bytes[0] = static_cast<std::byte>(value >> 24 & 0xFF);
			bytes[1] = static_cast<std::byte>(value >> 16 & 0xFF);
			bytes[2] = static_cast<std::byte>(value >> 8 & 0xFF);
			bytes[3] = static_cast<std::byte>(value & 0xFF);
			return bytes;
		}

		template <HasOneBytes T> std::byte constexpr convert_1_byte(T value) {
			return static_cast<std::byte>(value);
		}

		template <HasTwoBytes T> TwoBytes constexpr convert_2_bytes(T value) {
			TwoBytes bytes;

			bytes[0] = static_cast<std::byte>(value >> 8 & 0xFF);
			bytes[1] = static_cast<std::byte>(value & 0xFF);
			return bytes;
		}
	} // namespace Core

	namespace Types {
		using Uint32tBytes = Core::FourBytes;
		using Uint16Bytes  = Core::TwoBytes;
		using DoubleBytes  = Core::EightBytes;
		using FloatBytes   = Core::FourBytes;
		using Uint64tBytes = Core::EightBytes;
		using StringBytes  = std::vector<std::byte>;
		using Uint8tByte   = std::byte;
	} // namespace Types

	Types::Uint32tBytes convert_uint32_t(std::uint32_t value);
	Types::Uint16Bytes  convert_uint16_t(std::uint16_t value);
	Types::DoubleBytes  convert_double(double value);
	Types::FloatBytes   convert_float(float value);
	Types::Uint64tBytes convert_uint64_t(std::uint64_t value);
	Types::Uint8tByte   convert_uint8_t(std::uint8_t value);

	Types::StringBytes convert_string(const std::string &string);
} // namespace Serializer
