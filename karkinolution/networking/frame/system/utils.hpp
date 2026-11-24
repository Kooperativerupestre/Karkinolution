#pragma once

#include "karkinolution/networking/frame/system/core_aliases.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <cstdint>
#include <karkinolution/networking/frame/system/contract.hpp>
#include <variant>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::System {
			namespace Utils {
				uint8_t constexpr cast_numeric(Detail::SubTypeVariant sub_type) {
					if (std::holds_alternative<SystemCallSubTypes>(sub_type)) {
						return static_cast<uint8_t>(std::get<SystemCallSubTypes>(sub_type));
					} else if (std::holds_alternative<SystemOutputSubTypes>(sub_type)) {
						return static_cast<uint8_t>(std::get<SystemOutputSubTypes>(sub_type));
					}
					KShared::Functional::unreachable_or_throw("Invalid sub type of frame (system) on cast numeric");
				}
			} // namespace Utils
		} // namespace Frames::System
	} // namespace Networking
} // namespace Karkinolution