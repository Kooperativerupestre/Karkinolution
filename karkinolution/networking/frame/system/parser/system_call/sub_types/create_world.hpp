#pragma once

#include "karkinolution/networking/frame/system/layout.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/deserializer.hpp"

#include <karkinolution/networking/frame/system/parser/internal.hpp>

namespace Karkinolution::Networking::Frames::System::Parser::SystemCall::ST::CreateWorld {
	template <KShared::Binary::ByteRange T> bool get_auto_connect(const T &bytes) {
		return KShared::Binary::Deserializer::read_bool(bytes,
														Layout::SystemCall::ST::CreateWorld::AutoConnect::OFFSET);
	}

	template <KShared::Binary::ByteRange T> std::vector<std::byte> get_payload(const T &bytes) {
		std::vector<std::byte> payload;
		payload.insert(
			payload.end(),
			bytes.begin() + Layout::SystemCall::ST::CreateWorld::Payload::OFFSET,
			bytes.begin() + Layout::SystemCall::ST::CreateWorld::Payload::OFFSET
				+ Internal::calculate_payload_size(bytes, Layout::SystemCall::ST::CreateWorld::FIXED_HEADER_SIZE));
		return payload;
	}

	constexpr std::uint32_t calculate_declared_size(std::uint32_t payload_size) {
		return Internal::calculate_declared_size(payload_size, Layout::SystemCall::ST::CreateWorld::FIXED_HEADER_SIZE);
	}

	constexpr std::uint32_t calculate_payload_size(std::uint32_t declared_size) {
		return Internal::calculate_payload_size(declared_size, Layout::SystemCall::ST::CreateWorld::FIXED_HEADER_SIZE);
	}

	template <KShared::Binary::ByteRange T> Internal::FrameExistence has_frame(const T &bytes) {
		return Internal::has_frame(bytes, Layout::SystemCall::ST::CreateWorld::FIXED_HEADER_SIZE);
	}
} // namespace Karkinolution::Networking::Frames::System::Parser::SystemCall::ST::CreateWorld
