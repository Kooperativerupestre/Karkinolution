#pragma once
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/port.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/sdk/src/client/pending_request.hpp"
#include "karkinolution/networking/sdk/src/client/protocol_session.hpp"
#include "karkinolution/networking/sdk/src/client/request_queue.hpp"
#include "karkinolution/networking/sdk/src/error.hpp"
#include "shared/core/error.hpp"
#include "shared/core/id.hpp"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/strand.hpp>
#include <atomic>
#include <iostream>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/networking/asio_types.hpp>
#include <string>

namespace KarkinolutionSDK {
	using RequestId       = Karkinolution::Binary::RequestId;
	using RequestSubTypes = Karkinolution::Binary::RequestSubTypes;
	using Resource        = Karkinolution::Binary::Resource;

	struct SendingState {
			std::vector<std::byte> bytes;
			bool                   is_sending = false;
	};

	namespace Client {
		inline constexpr std::size_t MAX_TEMPORARY_RESPONSE_SIZE = 8192;

		struct RequestData {
				RequestSubTypes                                              sub_type;
				Resource                                                     resource;
				std::vector<std::byte>                                       payload;
				Karkinolution::Binary::Frames::Models::Request::RequestExtra extra;
		};

		struct SystemRequestData {
				Karkinolution::Networking::Frames::SystemCallSubTypes sub_type;
				std::vector<std::byte>                                payload;
		};

		struct Config {
				std::string   host = "127.0.0.1";
				std::uint16_t port = Karkinolution::Networking::PORT;
		};

		using ClientSideResponseBuffer = Karkinolution::Networking::RawFrameBuffer;

		class AsyncOperationsRunning {
			private:

				std::mutex              mutex;
				int                     count = 0;
				std::condition_variable cv;

			public:

				void increment() {
					std::lock_guard lock{mutex};
					count++;
				}

				void decrement() {
					std::lock_guard lock{mutex};
					count--;
					if (count == 0) {
						cv.notify_all();
					}
				}

				int load() {
					std::lock_guard lock{mutex};
					return count;
				}

				void wait_until_zero() {
					std::unique_lock lock{mutex};
					cv.wait(lock, [this] {
						return count == 0;
					});
				}
		};

		using ResponseResult =
			std::expected<std::optional<ResponseFrameVariant>, KarkinolutionSDK::Protocol::CancelledError>;

		class Client {
			public:

				explicit Client(Karkinolution::Networking::IoContext &io,
								Config                                config = {},
								const std::string                    &name   = "Client")
					: io_context_(io)
					, socket_(io_context_)
					, strand_(asio::make_strand(io_context_))
					, config(config) {
					creation_number = client_count.fetch_add(1);
				}

				~Client();

				void connect();
				void disconnect();

				[[nodiscard]]
				bool is_connected() const noexcept {
					return connected;
				}

				// The request function makes a request, and blocks the thread until it gets a
				// response

				ResponseResult                 request(const RequestData &data);
				Protocol::SystemResponseResult request(const SystemRequestData &data);

				// This function stores the request in a queue, it doesn't block the thread
				RequestId send_without_waiting(const RequestData &data);
				// if the request wasn't responsed yet, the value is nullopt
				ResponseResult pop_response(RequestId id);

				int get_current_async_operations_running() {
					return current_async_operations_running.load();
				}

				// stops the thread tha calls, but doesn't stop the thread of Client
				void stop_new_async_operations();

				void wait_until_zero_async_operations() {
					return current_async_operations_running.wait_until_zero();
				}

				const std::string &get_name() const {
					return name;
				}

				std::string get_nickname() const {
					return std::format("{}-{}", name, creation_number);
				}

				int get_creation_number() const {
					return creation_number;
				}

				static int get_client_count() {
					return client_count.load();
				}

			private:

				inline static std::atomic<int> client_count = 0;

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

				int         creation_number;
				std::string name;


				Config                                config;
				Karkinolution::Networking::IoContext &io_context_;

				// Serializes every handler and posted task that touches the client state,
				// so no two threads access it at the same time.
				// All access to that state MUST go through this strand.
				asio::strand<asio::io_context::executor_type> strand_;
				asio::ip::tcp::socket                         socket_;


				std::atomic<KShared::Core::Id::BaseIdType> next_request_id = 0;


				// These two data structures below use mutex
				// So you can normally have two different threads executing this client
				// asynchronously

				KarkinolutionSDK::Protocol::ProtocolSession protocol_session;

				std::array<std::byte, MAX_TEMPORARY_RESPONSE_SIZE> temporary_responses;

				SendingState      sending_state;
				std::atomic<bool> connected = false;

				AsyncOperationsRunning current_async_operations_running;
				std::atomic<bool>      stopped_new_async_operations = false;
		};

		namespace Validator {
			void constexpr validate_is_connected(const Client &client) {
				if (!client.is_connected()) {
					throw ClientError("Client isn't connected");
				}
			}
		}; // namespace Validator

	} // namespace Client

} // namespace KarkinolutionSDK
