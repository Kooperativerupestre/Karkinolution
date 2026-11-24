#pragma once

#include "karkinolution/networking/frame/world/contract.hpp"

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			using RequestSubTypes     = Request::SubType;
			using ResponseSubTypes    = Response::SubType;
			using ErrorSubTypes       = Error::SubType;
			using FrameDeclaredSize   = DeclaredSize;
			using RequestId           = Request::Id;
			using Resource            = Request::Resource;
			using ResponseCardinality = Response::Cardinality;
			using BinaryTypes         = Type;
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
