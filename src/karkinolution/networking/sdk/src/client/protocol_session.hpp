#pragma once
#include "karkinolution/binary/frames/parser.hpp"

#include <karkinolution/networking/raw_frame_buffer.hpp>
#include <karkinolution/networking/sdk/src/client/pending_request.hpp>
#include <karkinolution/networking/sdk/src/client/request_queue.hpp>

namespace KarkinolutionSDK {
	namespace Protocol {
		class ProtocolSession {
			public:

				void receive(std::span<const std::byte> bytes);

				void add_request(const RequestFrame &request);
				void add_request(std::span<const std::byte> bytes);
				void add_response(std::span<const std::byte> response);
				std::optional<std::vector<std::byte>> pop_request();
				// if it doesn't have a response, return std::nullopt
				std::optional<ResponseFrame> pop_response(RequestId id);
				std::optional<ResponseFrame> await_response(RequestId id);
				void                         faill_pending_requests();

			private:

				void            sync_requests();
				PendingRequests pending_requests;
				RawFrameBuffer  incoming_responses;
				RequestQueue    requests;
		};
	} // namespace Protocol
} // namespace KarkinolutionSDK