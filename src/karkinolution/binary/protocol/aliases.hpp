#pragma once

#include "karkinolution/binary/protocol/contract.hpp"

#include <cstdint>
#include <type_traits>

namespace Karkinolution::Binary::Protocol::Aliases {

	template <typename T> using UnderlyingType = std::underlying_type_t<T>;

	using RequestId = uint64_t;

	using FrameDeclaredSize = uint32_t;
	using FrameType         = UnderlyingType<Protocol::Type>;
	using FrameSubType      = UnderlyingType<Protocol::Request::Operation>;

	namespace Request {
		using FrameResource  = UnderlyingType<Protocol::Request::Resource>;
		using FrameSeeTarget = UnderlyingType<Protocol::Request::SeeTarget>;
	} // namespace Request

	namespace Response {
		using Cardinality = UnderlyingType<Protocol::Response::Cardinality>;
	} // namespace Response

	namespace Error {
		using Code = UnderlyingType<Protocol::Error::Code>;
	} // namespace Error

} // namespace Karkinolution::Binary::Protocol::Aliases