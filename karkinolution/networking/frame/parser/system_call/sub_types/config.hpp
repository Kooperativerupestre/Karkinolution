#pragma once
#include "karkinolution/networking/frame/layout.hpp"
#include "shared/binary/byte_range.hpp"

#include <karkinolution/networking/frame/parser/internal.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Parser::SystemCall::ST::Config {
				template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
					std::vector<std::byte> payload;

					payload.insert(
						payload.end(),
						bytes.begin() + Layout::SystemCall::ST::Config::Payload::OFFSET,
						bytes.begin() + Layout::SystemCall::ST::Config::Payload::OFFSET
							+ Internal::calculate_payload_size(bytes,
															   Layout::SystemCall::ST::Config::FIXED_HEADER_SIZE));

					return payload;
				}

				constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
					return Internal::calculate_declared_size(payload_size,
															 Layout::SystemCall::ST::Config::FIXED_HEADER_SIZE);
				}

				constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
					return Internal::calculate_payload_size(declared_size,
															Layout::SystemCall::ST::Config::FIXED_HEADER_SIZE);
				}

				template <KShared::Binary::ByteRange T> constexpr std::uint32_t calculate_payload_size(const T &bytes) {
					return Internal::calculate_payload_size(bytes, Layout::SystemCall::ST::Config::FIXED_HEADER_SIZE);
				}

			} // namespace Parser::SystemCall::ST::Config
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution