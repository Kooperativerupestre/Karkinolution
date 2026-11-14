#pragma once

#include "karkinolution/networking/scope/layout.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/errors.hpp"

#include <format>
#include <magic_enum/magic_enum.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Scope {
			namespace Parser {
				template <KShared::Binary::ByteRange T> Scope get_type(const T &bytes) {
					if (bytes.size() < 1) {
						throw KShared::Binary::ByteError(
							"Karkinolution::Networking::Scope::Parser::get_type should called on "
							"bytes with size() >= 1");
					}

					const auto type = static_cast<Scope>(bytes[0]);
					if (!magic_enum::enum_contains(type)) {
						throw KShared::Binary::ByteError(
							std::format("Invalid scope of frame: {}", static_cast<uint8_t>(bytes[0])));
					}
					return type;
				}
			} // namespace Parser
		} // namespace Scope
	} // namespace Networking
} // namespace Karkinolution