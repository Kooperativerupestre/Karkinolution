#pragma once
#include "karkinolution/binary/byte_range.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/organism/entities/properties/properties.hpp"

namespace PropertiesDSI {
	GenericProperty constexpr deserialize(std::byte byte) {
		if (byte == static_cast<std::byte>(0x01)) {
			return Properties::Capabilities::Move::SWIMM;
		} else if (byte == static_cast<std::byte>(0x02)) {
			return Properties::Capabilities::Move::WALK;
		}
		throw ByteError("Unknown type of property");
	}

	template <ByteRange T>
	GenericProperty constexpr deserialize(const T &payload, std::size_t offset) {
		return deserialize(payload[offset]);
	}
} // namespace PropertiesDSI