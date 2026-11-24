#pragma once

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/models.hpp"
#include "karkinolution/networking/client_side/client/pending_request.hpp"
#include "karkinolution/networking/client_side/client/request_queue.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/frame/system/contract.hpp"
#include "karkinolution/networking/frame/system/models.hpp"
#include "karkinolution/networking/frame/system/parser/internal.hpp"
#include "karkinolution/networking/raw_frame_buffer.hpp"
#include "karkinolution/networking/scope/layout.hpp"
#include "karkinolution/networking/scope/parser.hpp"
#include "shared/binary/byte_range.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <cassert>
#include <cstddef>
#include <functional>
#include <iostream>
#include <karkinolution/networking/frame/system/parser/parser.hpp>
#include <memory>
#include <optional>
#include <span>
#include <utility>
#include <variant>
#include <vector>

#pragma once

#include <cassert>
#include <cstddef>
#include <format>
#include <functional>
#include <optional>
#include <span>
#include <utility>
#include <variant>
#include <vector>

namespace Karkinolution::Networking::ClientSide::Protocol {
	using FrameExistence = Frames::System::Parser::Internal::FrameExistence;

	template <typename Response, typename RequestId> class AbstractSession {


		public:

			using SelfPendingRequests = PendingRequests<Response, RequestId>;
			using ResponseResult      = SelfPendingRequests::ResponseResult;

		private:


			SelfPendingRequests pendings;


			/*
			The ProtocolSession frame state does not account for the frame scope. Therefore, depending on the rule of
			the SessionManager, a scope may be set but it may not yet have been considered incomplete here.

			*/

			RawFrameBuffer incoming_responses;
			RequestQueue   requests;


		protected:

			template <Scope::Scope scope> BufferExistence has_frame_() const {
				if constexpr (scope == Scope::Scope::SERVER) {
					return FrameBufferAnalyzer::has_frame(incoming_responses, scope);
				} else if constexpr (scope == Scope::Scope::WORLD) {
					return FrameBufferAnalyzer::has_frame(incoming_responses, scope);
				}
				KShared::Functional::unreachable_or_throw();
			}

			template <Scope::Scope scope> std::optional<std::vector<std::byte>> pop_complete_raw_frame_() {
				if constexpr (scope == Scope::Scope::SERVER) {

					const auto raw_frame = FrameBufferAnalyzer::get_frame(incoming_responses, Scope::Scope::SERVER);
					if (!raw_frame) {
						return std::nullopt;
					}
					for (size_t i = 0; i < raw_frame->size(); i++) {
						incoming_responses.pop_front(); // clears the buffer
					}
					return raw_frame;

				} else if constexpr (scope == Scope::Scope::WORLD) {
					const auto raw_frame = FrameBufferAnalyzer::get_frame(incoming_responses, Scope::Scope::WORLD);
					if (!raw_frame) {
						return std::nullopt;
					}
					assert(raw_frame);

					for (size_t i = 0; i < raw_frame->size(); i++) {
						incoming_responses.pop_front();
					}
					return raw_frame;
				}
				KShared::Functional::unreachable_or_throw();
			}

			template <Scope::Scope scope>
			std::optional<FrameBufferAnalyzer::EndOfFrame> get_end_of_frame_(std::span<const std::byte> new_bytes) {
				if constexpr (scope == Scope::Scope::SERVER) {
					return FrameBufferAnalyzer::get_end_of_frame(incoming_responses, new_bytes, Scope::Scope::SERVER);
				} else if constexpr (scope == Scope::Scope::WORLD) {
					return FrameBufferAnalyzer::get_end_of_frame(incoming_responses, new_bytes, Scope::Scope::WORLD);
				}
				KShared::Functional::unreachable_or_throw();
			}

		public:

			AbstractSession() {}

			void register_request(RequestId id) {
				if (!pendings.add_request(id)) {
					throw ClientError(std::format("Request ID {} is already pending", id));
				}
			}

			void enqueue_request(std::span<const std::byte> request) {
				requests.add(request);
			}

			void add_response(std::span<const std::byte> bytes);

			std::optional<std::vector<std::byte>> pop_raw_request() {
				return requests.pop_front();
			}

			std::optional<std::vector<std::byte>> pop_request_front() {
				return requests.pop_front();
			}

			ResponseResult pop_response(RequestId id) {
				return pendings.pop_response(id);
			}

			ResponseResult await_response(RequestId id) {
				return pendings.await_response(id);
			}

			void fail_pending_requests() {
				pendings.fail_all_by_disconnection();
			}

			bool deliver_response(const Response &response, RequestId request_id) {
				return pendings.add_response(response, request_id);
			}

			void receive(std::span<const std::byte> new_bytes) {
				incoming_responses.add(new_bytes);
			}
	};

	class WorldSession : public AbstractSession<Binary::Frames::Models::ResponseVariant, Binary::RequestId> {
		public:

			BufferExistence                                has_frame() const;
			std::optional<std::vector<std::byte>>          pop_complete_raw_frame();
			std::optional<FrameBufferAnalyzer::EndOfFrame> get_end_of_frame(std::span<const std::byte> new_bytes);
	};

	class SystemSession
		: public AbstractSession<Frames::System::Models::SystemOutput::SystemOutput, Frames::System::SystemCall::Id> {
		public:

			BufferExistence                                has_frame() const;
			std::optional<std::vector<std::byte>>          pop_complete_raw_frame();
			std::optional<FrameBufferAnalyzer::EndOfFrame> get_end_of_frame(std::span<const std::byte> new_bytes);
	};

	class SendingFrameState {
		private:

			std::optional<Scope::Scope> current_scope_      = std::nullopt;
			bool                        is_receiving_frame_ = false;

		public:

			SendingFrameState() {}

			std::optional<Scope::Scope> current_scope() const;

			bool is_receiving_frame() const;

			bool is_reseted() const;

			// returns feedback on whether it was reset or not (it is not reset if it was already reset)
			bool reset();

			// returns feedback on whether it actually set the new values (only works if the object has been reset)
			bool emplace(Scope::Scope new_scope);
	};

	class SimulationSessions {
		public:

			using WorldResponseResult  = WorldSession::ResponseResult;
			using SystemResponseResult = SystemSession::ResponseResult;

		private:

			using TargetSessionToSend = std::optional<
				std::variant<std::reference_wrapper<WorldSession>, std::reference_wrapper<SystemSession>>>;


			WorldSession  world_session;
			SystemSession system_session;

			void server_receive(std::span<const std::byte> new_bytes);

			void world_receive(std::span<const std::byte> new_bytes);

			void sync_world_requests();

			void sync_system_requests();

		public:


			// The functions for synchronizing the two sessions are defined in the session manager
			// because it was not possible to handle model parsing without workarounds
			// involving template-passed functions as well (which would add too much complexity).
			// Therefore, I chose to move the knowledge of which parsing functions should be used
			// precisely to the class that defines the semantics (World & Server).


			// The two synchronization functions have similar behavior. To make use of incoming_responses,
			// comments made in sync_world_requests, outside the parsing part, should also be understood
			// as documentation for that specific part in sync_server_requests as well


			template <Scope::Scope scope> void receive(std::span<const std::byte> new_bytes) {
				if constexpr (scope == Scope::Scope::SERVER) {
					server_receive(new_bytes);
					sync_system_requests();
					return;
				} else if constexpr (scope == Scope::Scope::WORLD) {
					world_receive(new_bytes);
					sync_world_requests();
					return;
				}
				std::unreachable();
			}

			template <Scope::Scope scope> BufferExistence has_frame() {
				if constexpr (scope == Scope::Scope::SERVER) {
					return system_session.has_frame();
				} else if constexpr (scope == Scope::Scope::WORLD) {
					return world_session.has_frame();
				}
				KShared::Functional::unreachable_or_throw();
			}

			template <Scope::Scope scope>
			std::optional<FrameBufferAnalyzer::EndOfFrame>
			get_end_of_frame_with_new_bytes(std::span<const std::byte> new_bytes) {
				if constexpr (scope == Scope::Scope::SERVER) {
					return system_session.get_end_of_frame(new_bytes);
				} else if constexpr (scope == Scope::Scope::WORLD) {
					return world_session.get_end_of_frame(new_bytes);
				}
				KShared::Functional::unreachable_or_throw();
			}

			// doesn't block caller thread. If it has response, send, if it doesn't have, doesn't send

			WorldResponseResult pop_world_response(Binary::RequestId id);

			// doesn't block caller thread. If it has response, send, if it doesn't have, doesn't send
			SystemResponseResult pop_system_response(Frames::System::SystemCall::Id id);

			// block the thread until the response arrives (stops if it reaches timeout)
			WorldResponseResult await_world_response(Binary::RequestId id);

			// block the thread until the response arrives (stops if it reaches timeout)
			SystemResponseResult await_system_response(Frames::System::SystemCall::Id id);

			void fail_pending_requests();

			SimulationSessions() {}

			void register_world_request(Binary::RequestId id);

			void register_system_request(Frames::System::SystemCall::Id id);

			void enqueue_world_request(std::span<const std::byte> new_bytes);

			void enqueue_system_request(std::span<const std::byte> new_bytes);

			std::optional<std::vector<std::byte>> pop_world_request_front();

			std::optional<std::vector<std::byte>> pop_system_request_front();
	};

	class GenericRawFrame {
		private:

			std::optional<std::vector<std::byte>> frame;
			std::optional<Scope::Scope>           scope;

		public:

			bool constexpr has_value() const {
				return frame.has_value();
			}

			std::optional<std::vector<std::byte>> copy_frame() const;

			const std::optional<std::vector<std::byte>> &see_frame() const;

			std::optional<Scope::Scope> see_scope() const;

			GenericRawFrame() {}

			bool emplace(const std::vector<std::byte> &raw_frame, Scope::Scope scope);

			GenericRawFrame(const std::vector<std::byte> &raw_frame, Scope::Scope scope);

			operator bool() const noexcept;
	};

	class SessionManager {
		public:


			using WorldResponseResult  = WorldSession::ResponseResult;
			using SystemResponseResult = SystemSession::ResponseResult;

		private:

			SendingFrameState  receiving_frame_state;
			SimulationSessions sessions;

			// returns whether it set the scope or not
			bool set_current_scope(std::byte byte);

			template <KShared::Binary::ByteRange T> bool set_current_scope(const T &bytes) {
				return set_current_scope(bytes[0]);
			}

			bool set_current_scope(std::span<const std::byte> bytes);

			bool receive_on_current_scoped_session(std::span<const std::byte> bytes);

			// look only at the buffer. If it returns null, it means there was no current scope
			std::optional<FrameExistence> has_frame_on_current_scoped_session();


		public:

			void receive(std::span<const std::byte> bytes);

			// doesn't block caller thread. If it has response, send, if it doesn't have, doesn't send


			WorldResponseResult pop_world_response(Binary::RequestId id);

			// doesn't block caller thread. If it has response, send, if it doesn't have, doesn't send


			SystemResponseResult pop_system_response(Frames::System::SystemCall::Id id);

			WorldResponseResult await_world_response(Binary::RequestId id);

			SystemResponseResult await_system_response(Frames::System::SystemCall::Id id);

			void register_world_request(Binary::RequestId id);

			void register_system_request(Frames::System::SystemCall::Id id);

			SessionManager() {}

			void fail_pending_requests();

			void enqueue_world_request(std::span<const std::byte> new_request);

			void enqueue_system_request(std::span<const std::byte> new_request);

			std::optional<std::vector<std::byte>> pop_world_request_front();

			std::optional<std::vector<std::byte>> pop_system_request_front();

			GenericRawFrame pop_request_front();
	};


} // namespace Karkinolution::Networking::ClientSide::Protocol