#include "karkinolution/networking/client_side/client/protocol_session.hpp"

#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "karkinolution/networking/frame/world/models.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

namespace Karkinolution::Networking::ClientSide::Protocol {

	BufferExistence WorldSession::has_frame() const {
		return has_frame_<Scope::Scope::WORLD>();
	}

	std::optional<std::vector<std::byte>> WorldSession::pop_complete_raw_frame() {
		return pop_complete_raw_frame_<Scope::Scope::WORLD>();
	}

	std::optional<FrameBufferAnalyzer::EndOfFrame>
	WorldSession::get_end_of_frame(std::span<const std::byte> new_bytes) {
		return get_end_of_frame_<Scope::Scope::WORLD>(new_bytes);
	}

	BufferExistence SystemSession::has_frame() const {
		return has_frame_<Scope::Scope::SERVER>();
	}

	std::optional<std::vector<std::byte>> SystemSession::pop_complete_raw_frame() {
		return pop_complete_raw_frame_<Scope::Scope::SERVER>();
	}

	std::optional<FrameBufferAnalyzer::EndOfFrame>
	SystemSession::get_end_of_frame(std::span<const std::byte> new_bytes) {
		return get_end_of_frame_<Scope::Scope::SERVER>(new_bytes);
	}

	std::optional<Scope::Scope> SendingFrameState::current_scope() const {
		return current_scope_;
	}

	bool SendingFrameState::is_receiving_frame() const {
		return is_receiving_frame_;
	}

	bool SendingFrameState::is_reseted() const {
		return !current_scope_ || !is_receiving_frame_;
	}

	bool SendingFrameState::reset() {
		if (is_reseted()) {
			return false;
		}

		current_scope_      = std::nullopt;
		is_receiving_frame_ = false;
		return true;
	}

	bool SendingFrameState::emplace(Scope::Scope new_scope) {
		if (!is_reseted()) {
			return false;
		}
		current_scope_      = new_scope;
		is_receiving_frame_ = true;
		return true;
	}

	void SimulationSessions::server_receive(std::span<const std::byte> new_bytes) {
		system_session.receive(new_bytes);
	}

	void SimulationSessions::world_receive(std::span<const std::byte> new_bytes) {
		world_session.receive(new_bytes);
	}

	void SimulationSessions::sync_world_requests() {
		while (const auto raw_frame = world_session.pop_complete_raw_frame()) {
			assert(raw_frame);

			const auto type = Frames::World::Parser::Internal::get_type(raw_frame.value());

			// This std::optional does not have optional semantics; it only serves
			// to allow us to decide its type below in the parsing checks
			// if no throw occurs, this variable should have a value
			std::optional<Frames::World::Models::ResponseVariant> response = std::nullopt;

			if (type == Frames::World::BinaryTypes::Response) {
				response.emplace(Frames::World::Parser::parse_response_frame(raw_frame.value()));
			} else if (type == Frames::World::BinaryTypes::Error) {
				response.emplace(Frames::World::Parser::parse_error_frame(raw_frame.value()));
			} else {
				throw ClientError("Client received a request frame from the server");
			}

			assert(response.has_value());

			Frames::World::RequestId request_id;

			if (type == Frames::World::BinaryTypes::Response) {
				request_id = std::get<Frames::World::Models::Response::Response>(response.value()).get_request_id();
			} else if (type == Frames::World::BinaryTypes::Error) {
				request_id = std::get<Frames::World::Models::Error::Error>(response.value()).get_request_id();
			} else {
				KShared::Functional::unreachable_or_throw();
			}
			const auto added = world_session.deliver_response(response.value(), request_id);
			if (!added) {
				std::cout << "Pending requests reached the max size\n";
			}
		}
	}

	void SimulationSessions::sync_system_requests() {
		while (const auto raw_frame = system_session.pop_complete_raw_frame()) {
			assert(raw_frame);

			const auto type = Frames::System::Parser::Internal::get_type(raw_frame.value());

			std::optional<Frames::System::Models::SystemOutput::SystemOutput> response = std::nullopt;

			if (type == Frames::System::Type::SystemOutput) {
				response.emplace(Frames::System::Parser::parse_system_output_frame(raw_frame.value()));
			} else {
				throw ClientError("Client received a request frame from the server");
			}

			assert(response.has_value());
			const auto added = system_session.deliver_response(response.value(), response->get_call_id());
			if (!added) {
				std::cout << "Pending requests reached the max size\n";
			}
		}
	}

	SimulationSessions::WorldResponseResult SimulationSessions::pop_world_response(Frames::World::RequestId id) {
		return world_session.pop_response(id);
	}

	SimulationSessions::SystemResponseResult
	SimulationSessions::pop_system_response(Frames::System::SystemCall::Id id) {
		return system_session.pop_response(id);
	}

	SimulationSessions::WorldResponseResult SimulationSessions::await_world_response(Frames::World::RequestId id) {
		return world_session.await_response(id);
	}

	SimulationSessions::SystemResponseResult
	SimulationSessions::await_system_response(Frames::System::SystemCall::Id id) {
		return system_session.await_response(id);
	}

	void SimulationSessions::fail_pending_requests() {
		system_session.fail_pending_requests();
		world_session.fail_pending_requests();
	}

	void SimulationSessions::register_world_request(Frames::World::RequestId id) {
		return world_session.register_request(id);
	}

	void SimulationSessions::register_system_request(Frames::System::SystemCall::Id id) {
		return system_session.register_request(id);
	}

	void SimulationSessions::enqueue_world_request(std::span<const std::byte> new_bytes) {
		return world_session.enqueue_request(new_bytes);
	}

	void SimulationSessions::enqueue_system_request(std::span<const std::byte> new_bytes) {
		return system_session.enqueue_request(new_bytes);
	}

	std::optional<std::vector<std::byte>> SimulationSessions::pop_world_request_front() {
		return world_session.pop_request_front();
	}

	std::optional<std::vector<std::byte>> SimulationSessions::pop_system_request_front() {
		return system_session.pop_request_front();
	}

	std::optional<std::vector<std::byte>> GenericRawFrame::copy_frame() const {
		return frame;
	}

	const std::optional<std::vector<std::byte>> &GenericRawFrame::see_frame() const {
		return frame;
	}

	std::optional<Scope::Scope> GenericRawFrame::see_scope() const {
		return scope;
	}

	bool GenericRawFrame::emplace(const std::vector<std::byte> &raw_frame, Scope::Scope scope) {
		if (has_value()) {
			return false;
		}

		frame = raw_frame;
		scope = scope;
		return true;
	}

	GenericRawFrame::GenericRawFrame(const std::vector<std::byte> &raw_frame, Scope::Scope scope)
		: frame(raw_frame)
		, scope(scope) {}

	GenericRawFrame::operator bool() const noexcept {
		return has_value();
	}

	bool SessionManager::set_current_scope(std::byte byte) {
		Scope::Scope scope = Scope::Parser::get_type(byte);
		return receiving_frame_state.emplace(scope);
	}

	bool SessionManager::set_current_scope(std::span<const std::byte> bytes) {
		return set_current_scope(bytes[0]);
	}

	bool SessionManager::receive_on_current_scoped_session(std::span<const std::byte> bytes) {
		if (!receiving_frame_state.current_scope()) {
			return false;
		}

		switch (receiving_frame_state.current_scope().value()) {
			case Scope::Scope::SERVER:
				sessions.receive<Scope::Scope::SERVER>(bytes);
				break;

			case Scope::Scope::WORLD:
				sessions.receive<Scope::Scope::WORLD>(bytes);
				break;

			default:
				KShared::Functional::unreachable_or_throw();
		}

		return true;
	}

	std::optional<FrameExistence> SessionManager::has_frame_on_current_scoped_session() {
		if (!receiving_frame_state.is_receiving_frame()) {
			return std::nullopt;
		}

		switch (receiving_frame_state.current_scope().value()) {
			case Scope::Scope::SERVER:
				return sessions.has_frame<Scope::Scope::SERVER>();
			case Scope::Scope::WORLD:
				return sessions.has_frame<Scope::Scope::WORLD>();
			default:
				KShared::Functional::unreachable_or_throw();
		}
	}

	void SessionManager::receive(std::span<const std::byte> bytes) {
		if (bytes.empty()) {
			return;
		}

		if (!receiving_frame_state.is_receiving_frame()) {
			// The first byte of an inbound frame indicates the target scope (SERVER or WORLD).
			// We consume this scope byte to establish routing, and slice it off with subspan(1)
			// so that downstream session parsers receive pure frame bytes.
			set_current_scope(bytes);
			bytes = bytes.subspan(1);
			if (bytes.empty()) {
				return;
			}
		}

		assert(!receiving_frame_state.is_reseted());

		std::optional<FrameBufferAnalyzer::EndOfFrame> end_of_frame;

		switch (receiving_frame_state.current_scope().value()) {
			case Scope::Scope::SERVER:
				end_of_frame = sessions.get_end_of_frame_with_new_bytes<Scope::Scope::SERVER>(bytes);
				break;
			case Scope::Scope::WORLD:
				end_of_frame = sessions.get_end_of_frame_with_new_bytes<Scope::Scope::WORLD>(bytes);
				break;

			default:
				KShared::Functional::unreachable_or_throw();
		}

		// if the frame does not end even with the new bytes, then it is safe to add the new bytes
		if (!end_of_frame) {
			receive_on_current_scoped_session(bytes);
			if (has_frame_on_current_scoped_session() == FrameExistence::YES) {
				receiving_frame_state.reset();
				return;
			}
			return;
		}

		if (end_of_frame->place == FrameBufferAnalyzer::PlacesOfEndOfFrame::NEW_BYTES) {
			receive_on_current_scoped_session(bytes.first(end_of_frame->index));
			receiving_frame_state.reset();

			if (end_of_frame->index < bytes.size()) {
				receive(bytes.subspan(end_of_frame->index));
			}
		} else {
			receiving_frame_state.reset();
			return receive(bytes);
		}
	}

	SessionManager::WorldResponseResult SessionManager::pop_world_response(Frames::World::RequestId id) {
		return sessions.pop_world_response(id);
	}

	SessionManager::SystemResponseResult SessionManager::pop_system_response(Frames::System::SystemCall::Id id) {
		return sessions.pop_system_response(id);
	}

	SessionManager::WorldResponseResult SessionManager::await_world_response(Frames::World::RequestId id) {
		return sessions.await_world_response(id);
	}

	SessionManager::SystemResponseResult SessionManager::await_system_response(Frames::System::SystemCall::Id id) {
		return sessions.await_system_response(id);
	}

	void SessionManager::register_world_request(Frames::World::RequestId id) {
		return sessions.register_world_request(id);
	}

	void SessionManager::register_system_request(Frames::System::SystemCall::Id id) {
		return sessions.register_system_request(id);
	}

	void SessionManager::fail_pending_requests() {
		sessions.fail_pending_requests();
	}

	void SessionManager::enqueue_world_request(std::span<const std::byte> new_request) {
		return sessions.enqueue_world_request(new_request);
	}

	void SessionManager::enqueue_system_request(std::span<const std::byte> new_request) {
		return sessions.enqueue_system_request(new_request);
	}

	std::optional<std::vector<std::byte>> SessionManager::pop_world_request_front() {
		return sessions.pop_world_request_front();
	}

	std::optional<std::vector<std::byte>> SessionManager::pop_system_request_front() {
		return sessions.pop_system_request_front();
	}

	GenericRawFrame SessionManager::pop_request_front() {
		const auto world_request = pop_world_request_front();

		if (world_request) {
			return GenericRawFrame(world_request.value(), Scope::Scope::WORLD);
		}

		const auto system_request = pop_system_request_front();

		if (system_request) {
			return GenericRawFrame(system_request.value(), Scope::Scope::SERVER);
		}

		return GenericRawFrame();
	}

} // namespace Karkinolution::Networking::ClientSide::Protocol