#pragma once
#include "shared/binary/numeric_type.hpp"

#include <bit>
#include <cstdint>

namespace KShared {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace NLT {
					constexpr std::byte serialize_nlt_type(KShared::Binary::NumericType type) {
						return static_cast<std::byte>(static_cast<std::uint8_t>(type));
					}
				} // namespace NLT
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace KShared

namespace NLTSRI = KShared::Binary::Interpreters::SRI::NLT;
