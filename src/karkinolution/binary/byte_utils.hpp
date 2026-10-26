#pragma once
#include "byte_range.hpp"
#include "karkinolution/binary/core_aliases.hpp"

#include <bit>
#include <cstddef>
#include <deque>
#include <format>
#include <karkinolution/binary/protocol/aliases.hpp>
#include <karkinolution/binary/protocol/contract.hpp>
#include <karkinolution/core/error.hpp>
#include <vector>

namespace Karkinolution {
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
					throw ByteError(std::format(
						"Failed to apply zero padding on array that has {} size with offset {}",
						payload.size(),
						offset));
				}

				for (std::size_t i = offset; i < payload.size(); ++i) {
					payload[i] = std::byte{0};
				}
			}

			uint8_t constexpr cast_numeric(Resource resource) {
				return static_cast<uint8_t>(resource);
			}

			uint8_t constexpr cast_numeric(BinaryTypes type) {
				return static_cast<uint8_t>(type);
			}

			uint8_t constexpr cast_numeric(
				Karkinolution::Binary::Protocol::Detail::CodeVariant sub_type) {
				if (std::holds_alternative<RequestSubTypes>(sub_type)) {
					return static_cast<uint8_t>(std::get<RequestSubTypes>(sub_type));
				} else if (std::holds_alternative<ErrorSubTypes>(sub_type)) {
					return static_cast<uint8_t>(std::get<ErrorSubTypes>(sub_type));
				} else if (std::holds_alternative<ResponseSubTypes>(sub_type)) {
					return static_cast<uint8_t>(std::get<ResponseSubTypes>(sub_type));
				}
				throw ByteError("Invalid sub type");
			}

			template <ByteRange T>
			void append_bytes(std::vector<std::byte> &bytes, const T &value) {
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
			void
			append_bytes(std::array<std::byte, Size> &bytes, std::byte value, std::size_t offset) {
				bytes[offset] = value;
			}

			namespace NumericTypes {
				template <typename T> constexpr Karkinolution::Binary::Protocol::NumericType get() {

					if constexpr (std::is_same_v<T, float>) {
						return Karkinolution::Binary::Protocol::NumericType::FLOAT;
					} else if constexpr (std::is_same_v<T, double>) {
						return Karkinolution::Binary::Protocol::NumericType::DOUBLE;
					} else if constexpr (std::is_same_v<T, uint8_t>) {
						return Karkinolution::Binary::Protocol::NumericType::UINT8;
					} else if constexpr (std::is_same_v<T, uint32_t>) {
						return Karkinolution::Binary::Protocol::NumericType::UINT32;
					} else if constexpr (std::is_same_v<T, uint64_t>) {
						return Karkinolution::Binary::Protocol::NumericType::UINT64;
					} else if constexpr (std::is_same_v<T, int>) {
						return Karkinolution::Binary::Protocol::NumericType::INT;
					}

					throw ByteError(
						"Type doesn't exists as dynamic numeric language (binary) type of "
						"karkinolution");
				}
			} // namespace NumericTypes
		} // namespace Utils
	} // namespace Binary
} // namespace Karkinolution
