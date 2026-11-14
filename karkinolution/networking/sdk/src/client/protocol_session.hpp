#pragma once
#include "karkinolution/binary/frames/parser.hpp"

#include <karkinolution/networking/raw_frame_buffer.hpp>
#include <karkinolution/networking/sdk/src/client/pending_request.hpp>
#include <karkinolution/networking/sdk/src/client/request_queue.hpp>

namespace KarkinolutionSDK {
	namespace Protocol {
		using RequestId = Karkinolution::Binary::RequestId;

		class ProtocolSession {
			public:

				void receive(std::span<const std::byte> bytes);
				void register_request(RequestId id);
				void register_system_request(RequestId id);
				void enqueue_request(std::span<const std::byte> request);

				void                                  add_response(std::span<const std::byte> response);
				std::optional<std::vector<std::byte>> pop_request();

				KarkinolutionSDK::Protocol::ResponseResult pop_response(RequestId id);
				KarkinolutionSDK::Protocol::ResponseResult await_response(RequestId id);
				SystemResponseResult                       await_system_response(RequestId id);
				void                                       fail_pending_requests();

			private:

				void                                      sync_requests();
				PendingRequests                           pending_requests;
				SystemPendingRequests                     system_pending_requests;
				Karkinolution::Networking::RawFrameBuffer incoming_responses;
				RequestQueue                              requests;
		};
	} // namespace Protocol
} // namespace KarkinolutionSDK
