#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/binary/protocol/layouts.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/sdk/src/client/pending_request.hpp"

#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/networking/sdk/src/client/protocol_session.hpp>
#include <karkinolution/networking/sdk/src/error.hpp>
#include <unistd.h>

using KarkinolutionSDK::Protocol::ProtocolSession;

void ProtocolSession::receive(std::span<const std::byte> bytes) {
	incoming_responses.add(bytes);
	sync_requests();
}

void ProtocolSession::sync_requests() {

	while (FrameBufferAnalyzer::has_frame(incoming_responses) == BufferExistence::YES) {

		const auto raw_frame = FrameBufferAnalyzer::get_frame(incoming_responses).value();
		for (std::size_t i = 0; i < raw_frame.size(); ++i) {
			incoming_responses.pop_front();
		}

		const auto           type = Karkinolution::Binary::Frames::Parser::get_type(raw_frame);
		ResponseFrameVariant frame;
		if (type == BinaryTypes::Response) {
			frame = Karkinolution::Binary::Frames::Parser::parse_response_frame(raw_frame);
		} else if (type == BinaryTypes::Error) {
			frame = Karkinolution::Binary::Frames::Parser::parse_error_frame(raw_frame);
		} else {
			throw ClientError("Client received a request frame from the server");
		}

		const auto added = pending_requests.add_response(frame);

		if (!added) {
			std::cout << "Pending requests reached the max size\n";
		}
	}
}

void ProtocolSession::register_request(RequestId id) {
	if (!pending_requests.add_request(id)) {
		throw ClientError(std::format("Request ID {} is already pending", id));
	}
}

void ProtocolSession::enqueue_request(std::span<const std::byte> request) {
	requests.add(request);
}

std::optional<std::vector<std::byte>> ProtocolSession::pop_request() {
	return requests.pop_front();
}

KarkinolutionSDK::Protocol::ResponseResult ProtocolSession::pop_response(RequestId id) {
	return pending_requests.pop_response(id);
}

KarkinolutionSDK::Protocol::ResponseResult ProtocolSession::await_response(RequestId id) {
	return pending_requests.await_response(id);
}

void ProtocolSession::fail_pending_requests() {
	pending_requests.fail_all_by_disconnection();
}