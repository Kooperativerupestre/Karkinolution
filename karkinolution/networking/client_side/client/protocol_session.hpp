#pragma once

#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/client_side/client/pending_request.hpp"
#include "karkinolution/networking/client_side/client/request_queue.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"

#include <optional>
#include <span>
#include <vector>

namespace Karkinolution::Networking::ClientSide::Protocol {
	using RequestId = Karkinolution::Binary::RequestId;

	class ProtocolSession {
		public:

			void receive(std::span<const std::byte> bytes);
			void register_request(RequestId id);
			void register_system_request(RequestId id);
			void enqueue_request(std::span<const std::byte> request);

			void                                  add_response(std::span<const std::byte> response);
			std::optional<std::vector<std::byte>> pop_request();

			ResponseResult       pop_response(RequestId id);
			ResponseResult       await_response(RequestId id);
			SystemResponseResult await_system_response(RequestId id);
			void                 fail_pending_requests();

		private:

			void                                      sync_requests();
			PendingRequests                           pending_requests;
			SystemPendingRequests                     system_pending_requests;
			Karkinolution::Networking::RawFrameBuffer incoming_responses;
			RequestQueue                              requests;
	};
} // namespace Karkinolution::Networking::ClientSide::Protocol
