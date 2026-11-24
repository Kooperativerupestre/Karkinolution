#pragma once

#include "karkinolution/networking/client_side/client/protocol_session.hpp"
#include "karkinolution/networking/client_side/client/request_queue.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/frame/system/contract.hpp"
#include "karkinolution/networking/frame/system/core_aliases.hpp"
#include "karkinolution/networking/port.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/scope/layout.hpp"
#include "shared/core/error.hpp"
#include "shared/core/id.hpp"

#include <asio/buffer.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/strand.hpp>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <iostream>
#include <karkinolution/networking/asio_types.hpp>
#include <karkinolution/networking/frame/world/core_aliases.hpp>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace Karkinolution::Networking::ClientSide {
	using RequestId       = Karkinolution::Networking::Frames::World::RequestId;
	using RequestSubTypes = Karkinolution::Networking::Frames::World::RequestSubTypes;
	using Resource        = Karkinolution::Networking::Frames::World::Resource;

	class SendingState {
		private:

			std::optional<std::vector<std::byte>> raw_frame_;
			bool                                  is_sending_   = false;
			std::optional<bool>                   has_finished_ = std::nullopt;

			// se for nullopt, não está mandando. Se tiver valor, ele responde se já terminou ou não de adicionar raw
			// bytes
			std::optional<Scope::Scope> scope_;

			void reset_();

		public:

			explicit operator bool() const noexcept;

			bool constexpr is_sending() const noexcept {
				return is_sending_;
			}

			bool constexpr is_prepared() const noexcept {
				return is_sending_ && scope_;
			}

			std::optional<bool> constexpr has_finished() const noexcept {
				return has_finished_;
			}

			bool reset_if_finished();

			asio::const_buffer take_buffer() const;

			bool prepare_sending_state(Scope::Scope scope);

			// retorna se realmente adicionou os novos bytes
			bool enqueue_raw_bytes(std::span<const std::byte> new_bytes);

			bool mark_as_finished();
	};

	namespace Client {
		inline constexpr std::size_t MAX_TEMPORARY_RESPONSE_SIZE = 8192;

		struct WorldRequestData {
				RequestSubTypes                                                         sub_type;
				Resource                                                                resource;
				std::vector<std::byte>                                                  payload;
				Karkinolution::Networking::Frames::World::Models::Request::RequestExtra extra;
		};

		struct SystemRequestData {
				Karkinolution::Networking::Frames::System::SystemCallSubTypes                  sub_type;
				std::vector<std::byte>                                                         payload;
				Karkinolution::Networking::Frames::System::Models::SystemCall::SystemCallExtra extra = std::monostate{};
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

				void increment();
				void decrement();
				int  load();
				void wait_until_zero();
		};

		using WorldResponseResult  = Protocol::SessionManager::WorldResponseResult;
		using SystemResponseResult = Protocol::SessionManager::SystemResponseResult;

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

				WorldResponseResult  request_world_scope(const WorldRequestData &data);
				SystemResponseResult request_system_scope(const SystemRequestData &data);


				RequestId                      send_without_waiting_world_scope(const WorldRequestData &data);
				Frames::System::SystemCall::Id send_without_waiting_system_scope(const SystemRequestData &data);

				WorldResponseResult  pop_response_world_scope(RequestId id);
				SystemResponseResult pop_response_system_scope(Frames::System::SystemCall::Id id);

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


				int         creation_number;
				std::string name;

				Config                                config;
				Karkinolution::Networking::IoContext &io_context_;

				asio::strand<asio::io_context::executor_type> strand_;
				asio::ip::tcp::socket                         socket_;

				std::atomic<KShared::Core::Id::BaseIdType> next_request_id = 0;

				Protocol::SessionManager session;

				std::array<std::byte, MAX_TEMPORARY_RESPONSE_SIZE> temporary_responses;

				SendingState      sending_state;
				std::atomic<bool> connected = false;

				AsyncOperationsRunning current_async_operations_running;
				std::atomic<bool>      stopped_new_async_operations = false;
		};

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

				/*ResponseResult                 request(const WorldRequestData &data);
				Protocol::SystemResponseResult request(const SystemRequestData &data);

				RequestId      send_without_waiting(const WorldRequestData &data);
				ResponseResult pop_response(RequestId id);
*/
				[[nodiscard]]
				const Config &get_config() const noexcept {
					return config;
				}

				[[nodiscard]]
				const std::string &get_name() const noexcept {
					return name;
				}

				WorldResponseResult  request_world_scope(const WorldRequestData &data);
				SystemResponseResult request_system_scope(const SystemRequestData &data);


				RequestId                      send_without_waiting_world_scope(const WorldRequestData &data);
				Frames::System::SystemCall::Id send_without_waiting_system_scope(const SystemRequestData &data);

				WorldResponseResult  pop_response_world_scope(RequestId id);
				SystemResponseResult pop_response_system_scope(Frames::System::SystemCall::Id id);

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

			void constexpr validate_is_connected(const RuntimeClient &client) {
				if (!client.is_connected()) {
					throw ClientError("RuntimeClient isn't connected");
				}
			}
		}; // namespace Validator

	} // namespace Client
} // namespace Karkinolution::Networking::ClientSide