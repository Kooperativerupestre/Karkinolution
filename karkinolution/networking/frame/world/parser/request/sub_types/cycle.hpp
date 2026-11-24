#pragma once
#include "karkinolution/networking/frame/world/protocol/layouts.hpp"
#include "shared/binary/byte_range.hpp"

#include <karkinolution/networking/frame/world/parser/internal.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Parser {
				namespace Request {
					namespace ST {

						namespace Cycle {
							template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
								std::vector<std::byte> payload;
								payload.insert(payload.end(),
											   bytes.begin() + ProtocolLayout::Request::Cycle::Payload::OFFSET,
											   bytes.begin() + ProtocolLayout::Request::Cycle::Payload::OFFSET
												   + Internal::calculate_payload_size(
													   bytes,
													   ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE));
								return payload;
							}

							constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
								return Internal::calculate_declared_size(
									payload_size,
									ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
							}

							constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
								return Internal::calculate_payload_size(
									declared_size,
									ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
							}

							template <KShared::Binary::ByteRange T>
							constexpr std::uint32_t calculate_payload_size(const T &bytes) {
								return Internal::calculate_payload_size(
									bytes,
									ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
							}

							template <KShared::Binary::ByteRange T>
							constexpr Internal::FrameExistence has_frame(const T &bytes) {
								return Internal::has_frame(bytes, ProtocolLayout::Request::Cycle::FIXED_HEADER_SIZE);
							}

						} // namespace Cycle

					} // namespace ST
				} // namespace Request
			} // namespace Parser
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution