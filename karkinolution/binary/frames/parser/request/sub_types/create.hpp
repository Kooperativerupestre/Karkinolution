#pragma once
#include <karkinolution/binary/frames/parser/internal.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Parser {
				namespace Request {
					namespace ST {

						namespace Create {
							template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
								std::vector<std::byte> payload;
								payload.insert(payload.end(),
											   bytes.begin() + ProtocolLayout::Request::Create::Payload::OFFSET,
											   bytes.begin() + ProtocolLayout::Request::Create::Payload::OFFSET
												   + Internal::calculate_payload_size(
													   bytes,
													   ProtocolLayout::Request::Create::FIXED_HEADER_SIZE));
								return payload;
							}

							constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
								return Internal::calculate_declared_size(
									payload_size,
									ProtocolLayout::Request::Create::FIXED_HEADER_SIZE);
							}

							constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
								return Internal::calculate_payload_size(
									declared_size,
									ProtocolLayout::Request::Create::FIXED_HEADER_SIZE);
							}

							template <KShared::Binary::ByteRange T>
							constexpr std::uint32_t calculate_payload_size(const T &bytes) {
								return Internal::calculate_payload_size(
									bytes,
									ProtocolLayout::Request::Create::FIXED_HEADER_SIZE);
							}

						} // namespace Create

					} // namespace ST
				} // namespace Request
			} // namespace Parser
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution