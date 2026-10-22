#pragma once
#include <bit>
#include <cstdint>
#include <karkinolution/binary/protocol/contract.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Interpreters {
			namespace SRI {
				namespace NLT {
					constexpr std::byte serialize_nlt_type(Protocol::Types::BinaryNLT type) {
						return static_cast<std::byte>(static_cast<std::uint8_t>(type));
					}
				} // namespace NLT
			} // namespace SRI
		} // namespace Interpreters
	} // namespace Binary
} // namespace Karkinolution

namespace NLTSRI = Karkinolution::Binary::Interpreters::SRI::NLT;
