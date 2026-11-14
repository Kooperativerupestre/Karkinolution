#pragma once
#include "karkinolution/networking/frame/layout.hpp"

#include <karkinolution/binary/byte_range.hpp>
#include <karkinolution/binary/deserializer.hpp>
#include <karkinolution/networking/frame/contract.hpp>
#include <karkinolution/networking/frame/parser/internal.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames {
			namespace Parser {
				namespace SystemOutput {
					template <Binary::ByteRange T> Frames::SystemCall::Id get_id(const T &bytes) {
						return Binary::Deserializer::deserialize<Frames::SystemCall::Id>(
							bytes,
							Layout::SystemOutput::CallId::OFFSET);
					}

					template <Binary::ByteRange T> std::vector<std::byte> generic_get_payload(const T &bytes) {
						std::vector<std::byte> payload;
						payload.insert(
							payload.end(),
							bytes.begin() + Layout::SystemOutput::ST::AllWorldIds::Payload::OFFSET,
							bytes.begin() + Layout::SystemOutput::ST::AllWorldIds::Payload::OFFSET
								+ Internal::calculate_payload_size(bytes, Layout::SystemOutput::FIXED_HEADER_SIZE));
						return payload;
					}

					constexpr std::uint32_t generic_calculate_declared_size(std::uint32_t payload_size) {
						return Internal::calculate_declared_size(payload_size, Layout::SystemOutput::FIXED_HEADER_SIZE);
					}

					constexpr std::uint32_t generic_calculate_payload_size(std::uint32_t declared_size) {
						return Internal::calculate_payload_size(declared_size, Layout::SystemOutput::FIXED_HEADER_SIZE);
					}

					template <Binary::ByteRange T>
					constexpr std::uint32_t generic_calculate_payload_size(const T &bytes) {
						return Internal::calculate_payload_size(bytes, Layout::SystemOutput::FIXED_HEADER_SIZE);
					}


				} // namespace SystemOutput
			} // namespace Parser
		} // namespace Frames
	} // namespace Networking
} // namespace Karkinolution