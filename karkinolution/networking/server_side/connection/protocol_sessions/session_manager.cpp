#include "karkinolution/networking/server_side/connection/protocol_sessions/session_manager.hpp"

#include "karkinolution/networking/frame/parser/parser.hpp"
#include "karkinolution/networking/scope/parser.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <stdexcept>
#include <vector>

namespace Karkinolution::Networking::ServerSide::Connection::ProtocolSessions {
	void SessionManager::receive(std::span<const std::byte> bytes) {
		if (!incoming.add(bytes)) {
			throw std::overflow_error("SessionManager input buffer overflow");
		}
		process_frames();
	}

	void SessionManager::process_frames() {
		while (!incoming.view().empty()) {
			const auto             scope = Scope::Parser::get_type(incoming.view());
			std::vector<std::byte> frame_bytes;
			frame_bytes.reserve(incoming.view().size() - 1);
			frame_bytes.insert(frame_bytes.end(), incoming.view().begin() + 1, incoming.view().end());

			RawFrameBuffer frame_buffer;
			frame_buffer.add(frame_bytes);
			if (scope == Scope::Scope::SERVER) {
				if (Frames::Parser::has_frame(frame_buffer.view(), Frames::Layout::SystemCall::FIXED_HEADER_SIZE)
					!= Frames::Parser::FrameExistence::YES) {
					return;
				}
				const auto frame = Frames::Parser::get_frame(frame_buffer.view());
				server_session.receive(std::span<const std::byte>{frame});
				for (std::size_t index = 0; index < frame.size() + 1; ++index) {
					incoming.pop_front();
				}
				if (server_session.has_world() && !world_session) {
					world_session.emplace(server_session.world());
				}
			} else if (scope == Scope::Scope::WORLD) {
				if (FrameBufferAnalyzer::has_frame(frame_buffer) != BufferExistence::YES) {
					return;
				}
				if (!world_session) {
					throw std::runtime_error("World scope received before world acquisition");
				}
				const auto frame = FrameBufferAnalyzer::get_frame(frame_buffer);
				world_session->receive(std::span<const std::byte>{*frame});
				for (std::size_t index = 0; index < frame->size() + 1; ++index) {
					incoming.pop_front();
				}
			}
		}
	}

	bool SessionManager::has_response() const {
		return server_session.has_response() || (world_session && world_session->has_response());
	}

	SessionResponse SessionManager::pop_response() {
		if (server_session.has_response()) {
			return server_session.pop_response();
		}
		if (world_session && world_session->has_response()) {
			return world_session->pop_response();
		}
		throw std::runtime_error("SessionManager response queue is empty");
	}
} // namespace Karkinolution::Networking::ServerSide::Connection::ProtocolSessions
