#pragma once
#include <karkinolution/networking/frame/world/protocol/aliases.hpp>
#include <karkinolution/networking/frame/world/protocol/contract.hpp>
#include <karkinolution/networking/frame/world/protocol/layouts.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace ProtocolLayout = Karkinolution::Networking::Frames::World::Protocol::Layout;

			using Karkinolution::Networking::Frames::World::Protocol::NumericType;
			using Karkinolution::Networking::Frames::World::Protocol::Aliases::FrameDeclaredSize;

			using Karkinolution::Networking::Frames::World::Protocol::Aliases::RequestId;
			using Karkinolution::Networking::Frames::World::Protocol::Request::Resource;
			using BinaryTypes         = Karkinolution::Networking::Frames::World::Protocol::Type;
			using ResponseCardinality = Karkinolution::Networking::Frames::World::Protocol::Response::Cardinality;

			using RequestSubTypes  = Karkinolution::Networking::Frames::World::Protocol::Request::Operation;
			using ResponseSubTypes = Karkinolution::Networking::Frames::World::Protocol::Response::Code;
			using ErrorSubTypes    = Karkinolution::Networking::Frames::World::Protocol::Error::Code;
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution
