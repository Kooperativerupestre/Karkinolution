#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"

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

		const auto request_id = Frames::Parser::Response::get_id(raw_frame);
		const auto type       = Frames::Parser::get_type(raw_frame);
		const auto v_sub_type = Frames::Parser::get_sub_type(raw_frame);
		const auto payload    = Frames::Parser::get_payload(raw_frame);

		GenericResponseSubTypes sub_type;

		if (std::holds_alternative<RequestSubTypes>(v_sub_type)) {
			throw ClientError("Response frame cannot have request subtype");
		} else if (std::holds_alternative<ErrorSubTypes>(v_sub_type)) {
			sub_type = std::get<ErrorSubTypes>(v_sub_type);
		} else if (std::holds_alternative<ResponseSubTypes>(v_sub_type)) {
			sub_type = std::get<ResponseSubTypes>(v_sub_type);
		}

		const auto frame = Frames::Motor::make_response(type, sub_type, request_id, payload);

		const auto added = pending_requests.add_response(frame);

		if (!added) {
			return;
		}
	}
}

void ProtocolSession::add_request(std::span<const std::byte> request) {
	const auto id = Frames::Parser::Request::get_id(request);
	if (!pending_requests.add_request(id)) {
		throw ClientError(std::format("Request ID {} is already pending", id));
	}

	auto added_on_queue = requests.add(request);
}

void ProtocolSession::add_request(const RequestFrame &request) {
	if (!pending_requests.add_request(request.id)) {
		throw ClientError(std::format("Request ID {} is already pending", request.id));
	}

	auto added_on_queue = requests.add(Frames::Motor::make_request_raw_bytes(request.sub_type,
																			 request.id,
																			 request.resource,
																			 request.payload));

	if (!added_on_queue) {
		pending_requests.cancel(request.id);
		throw BufferError("The queue of requests is full");
	}
}

std::optional<std::vector<std::byte>> ProtocolSession::pop_request() {
	return requests.pop_front();
}

std::optional<ResponseFrame> ProtocolSession::pop_response(RequestId id) {
	return pending_requests.pop_response(id);
}

std::optional<ResponseFrame> ProtocolSession::await_response(RequestId id) {
	return pending_requests.await_response(id);
}

void ProtocolSession::faill_pending_requests() {
	pending_requests.faill_all();
}