#pragma once

#include "karkinolution/networking/frame/world/contract.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/binary/errors.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <cstdint>
#include <variant>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Utils {

				constexpr uint8_t cast_numeric(Request::Resource resource) {
					return static_cast<uint8_t>(resource);
				}

				constexpr uint8_t cast_numeric(Type type) {
					return static_cast<uint8_t>(type);
				}

				constexpr uint8_t cast_numeric(Detail::SubTypeVariant sub_type) {
					if (std::holds_alternative<Request::SubType>(sub_type)) {
						return static_cast<uint8_t>(std::get<Request::SubType>(sub_type));
					}
					if (std::holds_alternative<Error::SubType>(sub_type)) {
						return static_cast<uint8_t>(std::get<Error::SubType>(sub_type));
					}
					if (std::holds_alternative<Response::SubType>(sub_type)) {
						return static_cast<uint8_t>(std::get<Response::SubType>(sub_type));
					}
					KShared::Functional::unreachable_or_throw("Invalid sub type of frame (world) on cast numeric");
				}

			} // namespace Utils
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
