#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/frames/processor.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"

#include <iomanip>
#include <iostream>
#include <karkinolution/networking/connection/protocol_sessions/world.hpp>

namespace Karkinolution::Networking {
	namespace Connection {
		namespace ProtocolSessions {
			void WorldSession::clear_requests() {
				request_buffer.clear();
			}

			void WorldSession::on_overflow_error(std::span<const std::byte> bytes) const {
				std::cerr << "Overflow error: " << bytes.size() << " (discarded bytes) + "
						  << request_buffer.view().size() << " (request buffer size) > " << MAX_RAW_FRAME_BUFFER_SIZE
						  << "\n";

				for (const auto byte : bytes) {
					std::cerr << std::hex << std::setw(2) << std::setfill('0') << std::to_integer<unsigned int>(byte)
							  << ' ';
				}

				std::cerr << std::dec << '\n';
			}

			void WorldSession::receive(std::span<const std::byte> bytes) {
				if (bytes.size() + request_buffer.view().size() > MAX_RAW_FRAME_BUFFER_SIZE) {

					on_overflow_error(bytes);
					return;
				}

				for (const auto byte : bytes) {
					request_buffer.add(byte);
				}

				process_frames();
			}

			void WorldSession::process_frames() {
				while (FrameBufferAnalyzer::has_frame(request_buffer) == BufferExistence::YES) {

					auto frame = FrameBufferAnalyzer::get_frame(request_buffer);
					for (std::size_t i = 0; i < frame->size(); ++i) {
						request_buffer.pop_front();
					}

					auto request = Karkinolution::Binary::Frames::Parser::parse_request_frame(frame.value());

					auto response = Karkinolution::Binary::Frames::Processor::process(request, world);

					responses.add(std::move(response));
				}
			}

			bool WorldSession::has_response() const {
				return !responses.view().empty();
			}

			ResponseFrame WorldSession::pop_response() {
				return responses.pop_front();
			}

		} // namespace ProtocolSessions
	} // namespace Connection


} // namespace Karkinolution::Networking