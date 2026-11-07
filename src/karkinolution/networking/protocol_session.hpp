#pragma once
#include "karkinolution/binary/frames/parser.hpp"
#include "shared/math/units.hpp"

#include <karkinolution/networking/raw_frame_buffer.hpp>
#include <karkinolution/networking/response_queue.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution::Networking {

	enum class ReceiveOverflowMode : uint8_t {
		VERIFY_ALL_BYTES,
		VERIFY_BYTE_PER_BYTE
	};

	inline constexpr ReceiveOverflowMode GLOBAL_RECEIVE_OVERFLOW_MODE = ReceiveOverflowMode::VERIFY_ALL_BYTES;

	class ProtocolSession {
		public:

			explicit ProtocolSession(Karkinolution::World::World &world)
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

} // namespace Karkinolution::Networking
