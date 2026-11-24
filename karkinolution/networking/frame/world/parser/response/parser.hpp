#pragma once
#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "karkinolution/networking/frame/world/layout.hpp"

#include <karkinolution/networking/frame/world/parser/internal.hpp>
#include <vector>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Parser {

				namespace Response {
					constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
						return Internal::calculate_declared_size(payload_size,
																 Layout::Response::GenericPayload::OFFSET);
					}

					constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
						return Internal::calculate_payload_size(declared_size,
																Layout::Response::GenericPayload::OFFSET);
					}

					template <KShared::Binary::ByteRange T>
					constexpr std::uint32_t calculate_payload_size(const T &bytes) {
						return Internal::calculate_payload_size(bytes, Layout::Response::GenericPayload::OFFSET);
					}

					template <KShared::Binary::ByteRange T> RequestId get_request_id(const T &bytes) {
						return KShared::Binary::Deserializer::deserialize<RequestId>(
							bytes,
							Layout::Response::RequestId::OFFSET);
					}

					template <KShared::Binary::ByteRange T> ResponseCardinality get_cardinality(const T &bytes) {
						return static_cast<ResponseCardinality>(
							KShared::Binary::Deserializer::deserialize<Detail::UnderlyingType<ResponseCardinality>>(
								bytes,
								Layout::Response::Cardinality::OFFSET));
					}

					template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
						std::vector<std::byte> payload;
						payload.insert(
							payload.end(),
							bytes.begin() + Layout::Response::GenericPayload::OFFSET,
							bytes.begin() + Layout::Response::GenericPayload::OFFSET
								+ Internal::calculate_payload_size(bytes, Layout::Response::GenericPayload::OFFSET));
						return payload;
					}

					template <KShared::Binary::ByteRange T> Internal::FrameExistence has_frame(const T &bytes) {
						return Internal::has_frame(bytes, Layout::Response::FIXED_HEADER_SIZE);
					}

				} // namespace Response
			} // namespace Parser
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
