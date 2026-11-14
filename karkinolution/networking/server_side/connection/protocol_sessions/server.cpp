#include "karkinolution/networking/server_side/connection/protocol_sessions/server.hpp"

#include "karkinolution/networking/raw_frame_buffer.hpp"

#include <iomanip>
#include <iostream>
#include <karkinolution/networking/frame/parser/parser.hpp>
#include <karkinolution/networking/frame/processor.hpp>
#include <span>
#include <stdexcept>

namespace Karkinolution::Networking::ServerSide::Connection::ProtocolSessions {
	void ServerSession::clear_calls() {
		call_buffer.clear();
	}

	void ServerSession::on_overflow_error(std::span<const std::byte> bytes) const {
		std::cerr << "Overflow error: " << bytes.size() << " (discarded bytes) + " << call_buffer.view().size()
				  << " (request buffer size) > " << MAX_RAW_FRAME_BUFFER_SIZE << "\n";

		for (const auto byte : bytes) {
			std::cerr << std::hex << std::setw(2) << std::setfill('0') << std::to_integer<unsigned int>(byte) << ' ';
		}

		std::cerr << std::dec << '\n';
	}

	void ServerSession::receive(std::span<const std::byte> bytes) {
		if (bytes.size() + call_buffer.view().size() > MAX_RAW_FRAME_BUFFER_SIZE) {
			on_overflow_error(bytes);
			return;
		}

		for (const auto byte : bytes) {
			call_buffer.add(byte);
		}

		process_frames();
	}

	void ServerSession::process_frames() {
		while (Frames::Parser::has_frame(call_buffer.view(), Frames::Layout::SystemCall::FIXED_HEADER_SIZE)
			   == Frames::Parser::FrameExistence::YES) {
			const auto frame = Frames::Parser::get_frame(call_buffer.view());
			for (std::size_t index = 0; index < frame.size(); ++index) {
				call_buffer.pop_front();
			}
			const auto call = Frames::Parser::parse_system_call_frame(frame);
			system_outputs.add(Frames::Processor::process(call, simulation, world_lease));
		}
	}

	bool ServerSession::has_response() const {
		return !system_outputs.view().empty();
	}

	Frames::Models::SystemOutput::SystemOutput ServerSession::pop_response() {
		return system_outputs.pop_front();
	}

	bool ServerSession::has_world() const {
		return world_lease.has_value();
	}

	World::World &ServerSession::world() {
		if (!world_lease) {
			throw std::runtime_error("ServerSession has no acquired world");
		}
		return world_lease->get();
	}
} // namespace Karkinolution::Networking::ServerSide::Connection::ProtocolSessions
