#pragma once
#include "karkinolution/networking/frame/world/contract.hpp"
#include "karkinolution/networking/frame/world/layout.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/deserializer.hpp"

#include <karkinolution/networking/frame/world/parser/internal.hpp>
#include <vector>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Parser {
				namespace Request {
					namespace ST {
						namespace See {
							template <KShared::Binary::ByteRange T>
							Frames::World::Request::SeeTarget get_target(const T &bytes) {
								return static_cast<Frames::World::Request::SeeTarget>(
									KShared::Binary::Deserializer::deserialize<
										Detail::UnderlyingType<Frames::World::Request::SeeTarget>>(
										bytes,
										Layout::Request::ST::See::Target::OFFSET));
							}

							template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
								std::vector<std::byte> payload;
								payload.insert(payload.end(),
											   bytes.begin() + Layout::Request::ST::See::Payload::OFFSET,
											   bytes.begin() + Layout::Request::ST::See::Payload::OFFSET
												   + Internal::calculate_payload_size(
													   bytes,
													   Layout::Request::ST::See::FIXED_HEADER_SIZE));
								return payload;
							}

							constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
								return Internal::calculate_declared_size(payload_size,
																		 Layout::Request::ST::See::FIXED_HEADER_SIZE);
							}

							constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
								return Internal::calculate_payload_size(declared_size,
																		Layout::Request::ST::See::FIXED_HEADER_SIZE);
							}

							template <KShared::Binary::ByteRange T>
							constexpr std::uint32_t calculate_payload_size(const T &bytes) {
								return Internal::calculate_payload_size(bytes,
																		Layout::Request::ST::See::FIXED_HEADER_SIZE);
							}

							template <KShared::Binary::ByteRange T>
							constexpr Internal::FrameExistence has_frame(const T &bytes) {
								return Internal::has_frame(bytes, Layout::Request::ST::See::FIXED_HEADER_SIZE);
							}

						} // namespace See
					} // namespace ST
				} // namespace Request
			} // namespace Parser
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
