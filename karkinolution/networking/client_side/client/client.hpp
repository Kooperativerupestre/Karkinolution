#pragma once

#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/client_side/client/pending_request.hpp"
#include "karkinolution/networking/client_side/client/protocol_session.hpp"
#include "karkinolution/networking/client_side/client/request_queue.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/frame/core_aliases.hpp"
#include "karkinolution/networking/port.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "shared/core/error.hpp"
#include "shared/core/id.hpp"

#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/strand.hpp>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <karkinolution/binary/core_aliases.hpp>
#include <karkinolution/networking/asio_types.hpp>
#include <mutex>
#include <optional>
#include <string>
#include <thread>

namespace Karkinolution::Networking::ClientSide {
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

		using ResponseResult = std::expected<std::optional<ResponseFrameVariant>, Protocol::CancelledError>;

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

				ResponseResult                 request(const RequestData &data);
				Protocol::SystemResponseResult request(const SystemRequestData &data);

				RequestId      send_without_waiting(const RequestData &data);
				ResponseResult pop_response(RequestId id);

				int get_current_async_operations_running() {
					return current_async_operations_running.load();
				}

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
				void send();

				void clear_sending_state() {
					sending_state.is_sending = false;
					sending_state.bytes.clear();
				}

				int         creation_number;
				std::string name;

				Config                                config;
				Karkinolution::Networking::IoContext &io_context_;

				asio::strand<asio::io_context::executor_type> strand_;
				asio::ip::tcp::socket                         socket_;

				std::atomic<KShared::Core::Id::BaseIdType> next_request_id = 0;

				Protocol::ProtocolSession protocol_session;

				std::array<std::byte, MAX_TEMPORARY_RESPONSE_SIZE> temporary_responses;

				SendingState      sending_state;
				std::atomic<bool> connected = false;

				AsyncOperationsRunning current_async_operations_running;
				std::atomic<bool>      stopped_new_async_operations = false;
		};

		/// Isso é a interface mais acessível para quando o consumidor nao quer ter que lidar com io context
		/// diretamente, ele simplesmente usa essa classe e e agraciado pelo RAII.
		///
		/// This is the most accessible interface for callers who do not want to manage an io_context directly;
		/// they use this class and benefit from RAII lifecycle management.
		class RuntimeClient {
			public:

				explicit RuntimeClient(Config config = {}, std::string name = "RuntimeClient");
				~RuntimeClient() noexcept;

				RuntimeClient(const RuntimeClient &)            = delete;
				RuntimeClient &operator=(const RuntimeClient &) = delete;
				RuntimeClient(RuntimeClient &&)                 = delete;
				RuntimeClient &operator=(RuntimeClient &&)      = delete;

				[[nodiscard]]
				bool is_connected() const noexcept;

				bool connect() noexcept;
				bool disconnect() noexcept;

				ResponseResult                 request(const RequestData &data);
				Protocol::SystemResponseResult request(const SystemRequestData &data);

				RequestId      send_without_waiting(const RequestData &data);
				ResponseResult pop_response(RequestId id);

				[[nodiscard]]
				const Config &get_config() const noexcept {
					return config;
				}

				[[nodiscard]]
				const std::string &get_name() const noexcept {
					return name;
				}

			private:

				asio::io_context                                           io;
				asio::executor_work_guard<asio::io_context::executor_type> guard;
				Config                                                     config;
				std::string                                                name;
				std::optional<Client>                                      client;
				std::thread                                                runner;
		};

		namespace Validator {
			void constexpr validate_is_connected(const Client &client) {
				if (!client.is_connected()) {
					throw ClientError("Client isn't connected");
				}
			}
		}; // namespace Validator

	} // namespace Client
} // namespace Karkinolution::Networking::ClientSide
