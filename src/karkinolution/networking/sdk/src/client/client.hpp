#pragma once
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/networking/port.hpp"
#include "karkinolution/networking/protocol_session.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/sdk/src/client/pending_request.hpp"
#include "karkinolution/networking/sdk/src/client/protocol_session.hpp"
#include "karkinolution/networking/sdk/src/client/request_queue.hpp"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <atomic>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/binary/message_type_size.hpp>
#include <string>
inline constexpr std::size_t MAX_TEMPORARY_RESPONSE_SIZE = 4096;

namespace KarkinolutionSDK {
	struct SendingState {
			std::vector<std::byte> bytes;
			bool                   is_sending = false;
	};

	namespace Client {


		using ClientSideResponseBuffer = RawFrameBuffer;

		class Client {
			public:

				Client(std::string host);

				void connect();
				void disconnect();

				[[nodiscard]]
				bool is_connected() const noexcept {
					return connected;
				}

				// The request function makes a request, and blocks the thread until it gets a
				// response

				std::optional<ResponseFrame> request(RequestSubTypes            sub_type,
													 RequestResource            resource,
													 std::span<const std::byte> payload);

				// This function stores the request in a queue, it doesn't block the thread
				RequestId send_without_waiting(RequestSubTypes            sub_type,
											   RequestResource            resource,
											   std::span<const std::byte> payload);
				// if the request wasn't responsed yet, the value is nullopt
				std::optional<ResponseFrame> pop_response(RequestId id);

			private:

				void on_closed() {
					std::cout << "Client was closed\n";
					disconnect();
				}

				void on_error(const std::string &nessage);
				void receive();
				// This function go through the queue and send the bytes
				void send();

				void clear_sending_state() {
					sending_state.is_sending = false;
					sending_state.bytes.clear();
				}

				std::string   host_;
				std::uint16_t port = PORT;

				asio::io_context      io_context_;
				asio::ip::tcp::socket socket_;


				std::atomic<BaseIdType> next_request_id = 0;


				// These two data structures below use mutex
				// So you can normally have two different threads executing this client
				// asynchronously

				KarkinolutionSDK::Protocol::ProtocolSession protocol_session;

				std::array<std::byte, MAX_TEMPORARY_RESPONSE_SIZE> temporary_responses;

				SendingState sending_state;
				bool         connected = false;
		};

		namespace Validator {
			void constexpr validate_is_connected(const Client &client) {
				if (!client.is_connected()) {
					throw ConnectionError("Client isn't connected");
				}
			}
		}; // namespace Validator

	} // namespace Client

} // namespace KarkinolutionSDK