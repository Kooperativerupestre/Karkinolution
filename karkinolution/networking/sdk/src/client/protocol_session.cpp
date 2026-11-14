#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/binary/protocol/layouts.hpp"
#include "karkinolution/networking/frame/parser/parser.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/scope/parser.hpp"
#include "karkinolution/networking/sdk/src/client/pending_request.hpp"
#include "shared/core/error.hpp"

#include <iostream>
#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/networking/sdk/src/client/protocol_session.hpp>
#include <karkinolution/networking/sdk/src/error.hpp>
#include <unistd.h>

namespace KarkinolutionSDK::Protocol {

	void ProtocolSession::receive(std::span<const std::byte> bytes) {
		incoming_responses.add(bytes);
		sync_requests();
	}

	void ProtocolSession::sync_requests() {
		while (!incoming_responses.view().empty()) {
			const auto scope = Karkinolution::Networking::Scope::Parser::get_type(incoming_responses.view());
			std::vector<std::byte> unscoped{incoming_responses.view().begin() + 1, incoming_responses.view().end()};
			Karkinolution::Networking::RawFrameBuffer frame_buffer;
			frame_buffer.add(unscoped);
			std::vector<std::byte> raw_frame;
			if (scope == Karkinolution::Networking::Scope::Scope::SERVER) {
				if (Karkinolution::Networking::Frames::Parser::has_frame(
						frame_buffer.view(),
						Karkinolution::Networking::Frames::Layout::SystemOutput::FIXED_HEADER_SIZE)
					!= Karkinolution::Networking::Frames::Parser::FrameExistence::YES) {
					return;
				}
				raw_frame = Karkinolution::Networking::Frames::Parser::get_frame(frame_buffer.view());
			} else {
				if (Karkinolution::Networking::FrameBufferAnalyzer::has_frame(frame_buffer)
					!= Karkinolution::Networking::BufferExistence::YES) {
					return;
				}
				raw_frame = Karkinolution::Networking::FrameBufferAnalyzer::get_frame(frame_buffer).value();
			}
			for (std::size_t i = 0; i < raw_frame.size() + 1; ++i) {
				incoming_responses.pop_front();
			}
			if (scope == Karkinolution::Networking::Scope::Scope::SERVER) {
				system_pending_requests.add_response(
					Karkinolution::Networking::Frames::Parser::parse_system_output_frame(raw_frame));
				continue;
			}
			const auto           type  = Karkinolution::Binary::Frames::Parser::get_type(raw_frame);
			ResponseFrameVariant frame = [&]() -> ResponseFrameVariant {
				if (type == BinaryTypes::Response) {
					return Karkinolution::Binary::Frames::Parser::parse_response_frame(raw_frame);
				}
				return Karkinolution::Binary::Frames::Parser::parse_error_frame(raw_frame);
			}();
			pending_requests.add_response(frame);
		}
	}

	void ProtocolSession::register_request(RequestId id) {
		if (!pending_requests.add_request(id)) {
			throw ClientError(std::format("Request ID {} is already pending", id));
		}
	}

	void ProtocolSession::register_system_request(RequestId id) {
		if (!system_pending_requests.add_request(id)) {
			throw ClientError(std::format("System request ID {} is already pending", id));
		}
	}

	void ProtocolSession::enqueue_request(std::span<const std::byte> request) {
		requests.add(request);
	}

	std::optional<std::vector<std::byte>> ProtocolSession::pop_request() {
		return requests.pop_front();
	}

	ResponseResult ProtocolSession::pop_response(RequestId id) {
		return pending_requests.pop_response(id);
	}

	ResponseResult ProtocolSession::await_response(RequestId id) {
		return pending_requests.await_response(id);
	}

	SystemResponseResult ProtocolSession::await_system_response(RequestId id) {
		return system_pending_requests.await_response(id);
	}

	void ProtocolSession::fail_pending_requests() {
		pending_requests.fail_all_by_disconnection();
		system_pending_requests.fail_all_by_disconnection();
	}

} // namespace KarkinolutionSDK::Protocol
