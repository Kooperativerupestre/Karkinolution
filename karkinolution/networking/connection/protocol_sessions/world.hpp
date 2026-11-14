#pragma once

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/motor.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/scope/layout.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/binary/byte_utils.hpp"
#include "shared/math/units.hpp"

#include <karkinolution/networking/raw_frame_buffer.hpp>
#include <karkinolution/networking/response_queue.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking {
	namespace Connection {
		namespace ProtocolSessions {
			class WorldSession {
				public:

					explicit WorldSession(Karkinolution::World::World &world)
						: world(world) {}

					void receive(std::span<const std::byte> bytes);

					bool          has_response() const;
					ResponseFrame pop_response();


				private:

					Karkinolution::World::World &world;
					ResponseQueue                responses;
					RawFrameBuffer               request_buffer;

					void on_overflow_error(std::span<const std::byte> bytes) const;
					void process_frames();
					void clear_requests();
			};

		} // namespace ProtocolSessions
	} // namespace Connection

} // namespace Karkinolution::Networking
