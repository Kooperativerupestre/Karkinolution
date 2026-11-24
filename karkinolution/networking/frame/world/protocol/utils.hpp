#pragma once

#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "shared/binary/byte_utils.hpp"

#include <cstdint>
#include <variant>

namespace Karkinolution::Networking::Frames::World::Utils {

	constexpr uint8_t cast_numeric(Karkinolution::Networking::Frames::World::Resource resource) {
		return static_cast<uint8_t>(resource);
	}

	constexpr uint8_t cast_numeric(Karkinolution::Networking::Frames::World::BinaryTypes type) {
		return static_cast<uint8_t>(type);
	}

	constexpr uint8_t cast_numeric(Karkinolution::Networking::Frames::World::Protocol::Detail::CodeVariant sub_type) {
		if (std::holds_alternative<Karkinolution::Networking::Frames::World::RequestSubTypes>(sub_type)) {
			return static_cast<uint8_t>(std::get<Karkinolution::Networking::Frames::World::RequestSubTypes>(sub_type));
		}
		if (std::holds_alternative<Karkinolution::Networking::Frames::World::ErrorSubTypes>(sub_type)) {
			return static_cast<uint8_t>(std::get<Karkinolution::Networking::Frames::World::ErrorSubTypes>(sub_type));
		}
		if (std::holds_alternative<Karkinolution::Networking::Frames::World::ResponseSubTypes>(sub_type)) {
			return static_cast<uint8_t>(std::get<Karkinolution::Networking::Frames::World::ResponseSubTypes>(sub_type));
		}
		throw KShared::Binary::ByteError("Invalid sub type");
	}

} // namespace Karkinolution::Networking::Frames::World::Utils
