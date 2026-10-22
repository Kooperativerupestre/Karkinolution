#pragma once

#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/core/id.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace Error {
					namespace Layout {
						inline constexpr size_t ERROR_MAX_SIZE = 400;
					} // namespace Layout

					struct SerializedErrorOutput {
							std::vector<std::byte>                 payload;
							Protocol::Types::BinarySubTypes::Error sub_type;
					};

					std::vector<std::byte> serialize_error(const std::string &error);

					SerializedErrorOutput serialize_creature_was_not_found(BaseIdType id);
					SerializedErrorOutput serialize_corpse_was_not_found(BaseIdType id);
				} // namespace Error
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace ErrorSRI = Karkinolution::Binary::Interpreters::SRI::Error;
using SEO          = ErrorSRI::SerializedErrorOutput;
