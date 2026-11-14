#pragma once
#include <karkinolution/binary/binary_validators.hpp>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/deserializer.hpp>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Parser {
				namespace Request {
					template <ByteRange T> RequestId get_id(const T &bytes) {
						return Karkinolution::Binary::Deserializer::deserialize<RequestId>(
							bytes,
							ProtocolLayout::Request::Id::OFFSET);
					}

					template <ByteRange T> Resource get_resource(const T &bytes) {
						auto read = Karkinolution::Binary::Deserializer::deserialize<Protocol::Aliases::FrameSubType>(
							bytes,
							ProtocolLayout::Request::Resource::OFFSET);

						Karkinolution::Binary::Protocol::Validator::Request::validate_resource(read);
						return Resource(read);
					}


				} // namespace Request
			} // namespace Parser
		} // namespace Frames
	} // namespace Binary
} // namespace Karkinolution