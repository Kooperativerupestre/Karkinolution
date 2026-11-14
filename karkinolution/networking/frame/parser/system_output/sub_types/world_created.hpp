#pragma once

#include "karkinolution/networking/frame/layout.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/deserializer.hpp"

#include <karkinolution/networking/frame/parser/internal.hpp>

namespace Karkinolution::Networking::Frames::Parser::SystemOutput::ST::WorldCreated {
	template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
		std::vector<std::byte> payload;
		payload.insert(
			payload.end(),
			bytes.begin() + Layout::SystemOutput::ST::WorldCreated::Payload::OFFSET,
			bytes.begin() + Layout::SystemOutput::ST::WorldCreated::Payload::OFFSET
				+ Internal::calculate_payload_size(bytes, Layout::SystemOutput::ST::WorldCreated::FIXED_HEADER_SIZE));
		return payload;
	}

	constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
		return Internal::calculate_declared_size(payload_size,
												 Layout::SystemOutput::ST::WorldCreated::FIXED_HEADER_SIZE);
	}

	constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
		return Internal::calculate_payload_size(declared_size,
												Layout::SystemOutput::ST::WorldCreated::FIXED_HEADER_SIZE);
	}
} // namespace Karkinolution::Networking::Frames::Parser::SystemOutput::ST::WorldCreated
