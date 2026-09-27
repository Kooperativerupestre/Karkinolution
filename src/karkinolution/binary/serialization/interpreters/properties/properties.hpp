#pragma once
#include "karkinolution/core/error.hpp"

#include <karkinolution/organism/entities/properties/properties.hpp>

namespace PropertiesSRI {
	inline constexpr std::size_t PROPERTY_BYTES = 1;

	constexpr std::byte serialize(Properties::Capabilities::Move action) {
		if (action == Properties::Capabilities::Move::SWIMM) {
			return static_cast<std::byte>(0x01);
		} else if (action == Properties::Capabilities::Move::WALK) {
			return static_cast<std::byte>(0x02);
		}
		throw ByteError("Undefined type of move capabilities");
	}

	constexpr std::byte serialize(GenericProperty property) {
		if (std::holds_alternative<Properties::Capabilities::Move>(property)) {
			return serialize(std::get<Properties::Capabilities::Move>(property));
		}
		throw ByteError("Undefined type of property");
	}
} // namespace PropertiesSRI