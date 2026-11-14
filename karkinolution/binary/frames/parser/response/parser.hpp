#pragma once
#include <karkinolution/binary/frames/parser/internal.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Parser {

				namespace Response {
					constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
						return Internal::calculate_declared_size(payload_size,
																 ProtocolLayout::Response::Payload::OFFSET);
					}

					constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
						return Internal::calculate_payload_size(declared_size,
																ProtocolLayout::Response::Payload::OFFSET);
					}

					template <KShared::Binary::ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes) {
						return Internal::calculate_payload_size(bytes, ProtocolLayout::Response::Payload::OFFSET);
					}

					template <KShared::Binary::ByteRange T> RequestId get_request_id(const T &bytes) {
						return KShared::Binary::Deserializer::deserialize<RequestId>(
							bytes,
							ProtocolLayout::Response::RequestId::OFFSET);
					}

					template <KShared::Binary::ByteRange T> ResponseCardinality get_cardinality(const T &bytes) {
						return static_cast<ResponseCardinality>(
							KShared::Binary::Deserializer::deserialize<Protocol::Aliases::Response::Cardinality>(
								bytes,
								ProtocolLayout::Response::Cardinality::OFFSET));
					}

					template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
						std::vector<std::byte> payload;
						payload.insert(
							payload.end(),
							bytes.begin() + ProtocolLayout::Response::Payload::OFFSET,
							bytes.begin() + ProtocolLayout::Response::Payload::OFFSET
								+ Internal::calculate_payload_size(bytes, ProtocolLayout::Response::Payload::OFFSET));
						return payload;
					}


				} // namespace Response
			} // namespace Parser
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution