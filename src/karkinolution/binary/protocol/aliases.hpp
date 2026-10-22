#pragma once

#include "karkinolution/binary/protocol/contract.hpp"

#include <cstdint>
#include <type_traits>

namespace Karkinolution::Binary::Protocol::Aliases {

	template <typename T> using UnderlyingType = std::underlying_type_t<T>;

	using RequestId         = uint64_t;
	using FrameDeclaredSize = uint32_t;
	using FrameType         = UnderlyingType<Types::BinaryTypes>;
	using FrameSubType      = UnderlyingType<Types::BinarySubTypes::Request>;
	using BinaryNLTEnumType = UnderlyingType<Types::BinaryNLT>;

	using RequestResourceEnumType = UnderlyingType<Types::RequestResource>;
	using BinaryTypesEnumType     = UnderlyingType<Types::BinaryTypes>;
	using BinarySubTypesEnumType  = UnderlyingType<Types::BinarySubTypes::Error>;

} // namespace Karkinolution::Binary::Protocol::Aliases
