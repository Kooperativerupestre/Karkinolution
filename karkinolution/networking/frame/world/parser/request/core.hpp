#pragma once
#include <karkinolution/networking/frame/world/binary_validators.hpp>
#include <karkinolution/networking/frame/world/core_aliases.hpp>
#include <shared/binary/deserializer.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Parser {
				namespace Request {
					template <KShared::Binary::ByteRange T> RequestId get_id(const T &bytes) {
						return KShared::Binary::Deserializer::deserialize<RequestId>(
							bytes,
							ProtocolLayout::Request::Id::OFFSET);
					}

					template <KShared::Binary::ByteRange T> Resource get_resource(const T &bytes) {
						auto read = KShared::Binary::Deserializer::deserialize<Protocol::Aliases::FrameSubType>(
							bytes,
							ProtocolLayout::Request::Resource::OFFSET);

						Karkinolution::Networking::Frames::World::Protocol::Validator::Request::validate_resource(read);
						return Resource(read);
					}


				} // namespace Request
			} // namespace Parser
		} // namespace Frames::World
	} // namespace Networking
} // namespace Karkinolution