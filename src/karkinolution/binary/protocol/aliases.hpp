#pragma once

#include "karkinolution/binary/protocol/contract.hpp"

#include <cstdint>
#include <type_traits>

namespace Karkinolution::Binary::Protocol::Aliases {

	template <typename T> using UnderlyingType = std::underlying_type_t<T>;

	using RequestId = uint64_t;

	using FrameDeclaredSize = uint32_t;
	using FrameType         = UnderlyingType<Protocol::Type>;
	using FrameSubType      = UnderlyingType<Protocol::Operation>;
	using FrameResource     = UnderlyingType<Protocol::Resource>;

} // namespace Karkinolution::Binary::Protocol::Aliases