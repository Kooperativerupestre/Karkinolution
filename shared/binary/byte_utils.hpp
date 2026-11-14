#pragma once
#include "shared/binary/byte_range.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <deque>
#include <format>
#include <shared/binary/errors.hpp>
#include <vector>

namespace KShared {
	namespace Binary {
		namespace Utils {
			inline std::byte char_to_byte(char byte) {
				return static_cast<std::byte>(static_cast<unsigned char>(byte));
			}

			inline unsigned int byte_to_int(std::byte byte) {
				return std::to_integer<unsigned int>(byte);
			}

			template <ByteRange T> constexpr void zero_padding(T &payload, std::size_t offset) {
				if (offset > payload.size()) {
					throw ByteError(std::format("Failed to apply zero padding on array that has {} size with offset {}",
												payload.size(),
												offset));
				}

				for (std::size_t i = offset; i < payload.size(); ++i) {
					payload[i] = std::byte{0};
				}
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

			inline void append_bytes(std::vector<std::byte> &bytes, std::byte value, std::size_t offset) {
				bytes[offset] = value;
			}

			inline void append_bytes(std::deque<std::byte> &bytes, std::byte value) {
				bytes.push_back(value);
			}

			template <std::size_t Size>
			void append_bytes(std::array<std::byte, Size> &bytes, std::byte value, std::size_t offset) {
				bytes[offset] = value;
			}

		} // namespace Utils
	} // namespace Binary
} // namespace KShared
