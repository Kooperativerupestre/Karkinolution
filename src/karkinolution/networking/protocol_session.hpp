#pragma once
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/math/units.hpp"

#include <karkinolution/networking/port.hpp>
#include <karkinolution/networking/raw_frame_buffer.hpp>
#include <karkinolution/networking/response_queue.hpp>
#include <karkinolution/world/world.hpp>

enum class ReceiveOverflowMode : uint8_t {
	VERIFY_ALL_BYTES,
	VERIFY_BYTE_PER_BYTE
};

inline constexpr ReceiveOverflowMode GLOBAL_RECEIVE_OVERFLOW_MODE =
	ReceiveOverflowMode::VERIFY_ALL_BYTES;

void constexpr log_receive_overflow_mode() {
	if constexpr (GLOBAL_RECEIVE_OVERFLOW_MODE == ReceiveOverflowMode::VERIFY_ALL_BYTES) {
		std::cout << "The receive overflow mode is: verify all bytes before receive\n";
	} else if constexpr (GLOBAL_RECEIVE_OVERFLOW_MODE
						 == ReceiveOverflowMode::VERIFY_BYTE_PER_BYTE) {
		std::cout << "Karkinolution server is initializing 🦀 at port: " << PORT << "\n";
		std::cout << "The receive overflow mode is: verify byte per byte while receiving\n";
	}
}

class ProtocolSession {
	public:

		explicit ProtocolSession(World &world)
			: world(world) {}

		void receive(std::span<const std::byte> bytes);

		bool          has_response() const;
		ResponseFrame pop_response();


	private:

		World         &world;
		ResponseQueue  responses;
		RawFrameBuffer request_buffer;

		void on_overflow_error(std::span<const std::byte> bytes) const;
		void process_frames();
		void clear_requests();
};
