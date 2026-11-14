#pragma once

#include "karkinolution/binary/core_aliases.hpp"
#include "shared/binary/byte_utils.hpp"

#include <cstdint>
#include <variant>

namespace Karkinolution::Binary::Utils {

	constexpr uint8_t cast_numeric(Karkinolution::Binary::Resource resource) {
		return static_cast<uint8_t>(resource);
	}

	constexpr uint8_t cast_numeric(Karkinolution::Binary::BinaryTypes type) {
		return static_cast<uint8_t>(type);
	}

	constexpr uint8_t cast_numeric(Karkinolution::Binary::Protocol::Detail::CodeVariant sub_type) {
		if (std::holds_alternative<Karkinolution::Binary::RequestSubTypes>(sub_type)) {
			return static_cast<uint8_t>(std::get<Karkinolution::Binary::RequestSubTypes>(sub_type));
		}
		if (std::holds_alternative<Karkinolution::Binary::ErrorSubTypes>(sub_type)) {
			return static_cast<uint8_t>(std::get<Karkinolution::Binary::ErrorSubTypes>(sub_type));
		}
		if (std::holds_alternative<Karkinolution::Binary::ResponseSubTypes>(sub_type)) {
			return static_cast<uint8_t>(std::get<Karkinolution::Binary::ResponseSubTypes>(sub_type));
		}
		throw KShared::Binary::ByteError("Invalid sub type");
	}

} // namespace Karkinolution::Binary::Utils
