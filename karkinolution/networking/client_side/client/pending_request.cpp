#include "karkinolution/networking/client_side/client/pending_request.hpp"

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/frame/contract.hpp"
#include "shared/core/error.hpp"

#include <algorithm>
#include <chrono>
#include <format>
#include <stdexcept>

namespace Karkinolution::Networking::ClientSide::Protocol {
	bool PendingRequests::erase_request(Binary::RequestId id, const std::shared_ptr<SelfFutureResponse> expected) {
		std::lock_guard lock{mtx};
		const auto      it = pendings.find(id);
		if (it != pendings.end() && it->second == expected) {
			pendings.erase(it);
			return true;
		}
		return false;
	}

	bool PendingRequests::add_request(Binary::RequestId id) {
		auto future_response = std::make_shared<SelfFutureResponse>();

		std::lock_guard lock{mtx};
		return pendings.emplace(id, std::move(future_response)).second;
	}

	bool PendingRequests::add_response(const ResponseFrameVariant &response) {
		std::shared_ptr<SelfFutureResponse> future_response;
		{
			std::lock_guard lock{mtx};

			const auto request_id = std::visit(
				[](const auto &frame) {
					return frame.get_request_id();
				},
				response);
			auto it = pendings.find(request_id);
			if (it == pendings.end()) {
				throw ClientSideError(
					std::format("Invalid try to add a response to an inexistent request with id = {}", request_id));
			}
			future_response = it->second;
		}

		return future_response->set_response(response);
	}

	ResponseResult PendingRequests::pop_response(Binary::RequestId id) {
		std::shared_ptr<SelfFutureResponse> future_response;
		{
			std::lock_guard lock{mtx};
			auto            it = pendings.find(id);
			if (it == pendings.end()) {
				throw ClientSideError(std::format("Invalid pop response of an inexistent request with id = {}", id));
			}
			future_response = it->second;
		}
		auto response = future_response->try_get();

		if (!response || response->has_value()) {
			erase_request(id, future_response);
		}

		return response;
	}

	ResponseResult PendingRequests::await_response(Binary::RequestId id) {
		std::shared_ptr<SelfFutureResponse> future_response;

		{
			std::lock_guard lock{mtx};

			auto it = pendings.find(id);

			if (it == pendings.end()) {
				throw ClientSideError(std::format("Invalid await response of a nonexistent request with id = {}", id));
			}

			future_response = it->second;
		}

		auto result = future_response->await_response(std::chrono::seconds(RESPONSE_WAITING_TIME_OUT_SECONDS));

		erase_request(id, future_response);

		return result;
	}

	void PendingRequests::fail_all_by_disconnection() {
		std::vector<std::shared_ptr<SelfFutureResponse>> future_responses;

		{
			std::lock_guard lock{mtx};

			future_responses.reserve(pendings.size());

			for (const auto &[id, request] : pendings) {
				future_responses.push_back(request);
			}
		}

		for (const auto &request : future_responses) {
			request->fail(RequestFailure::DISCONNECTION);
		}
	}

	bool SystemPendingRequests::add_request(Frames::SystemCall::Id id) {
		auto pending    = std::make_shared<Pending>();
		pending->future = pending->promise.get_future();
		std::lock_guard lock{mtx};
		return promises.emplace(id, std::move(pending)).second;
	}

	bool SystemPendingRequests::add_response(const SystemOutput &response) {
		std::shared_ptr<Pending> pending;
		{
			std::lock_guard lock{mtx};
			const auto      it = promises.find(response.get_call_id());
			if (it == promises.end()) {
				throw ClientSideError(std::format("Invalid system response id {}", response.get_call_id()));
			}
			pending = it->second;
		}
		if (pending->failure) {
			return false;
		}
		pending->promise.set_value(response);
		return true;
	}

	SystemResponseResult SystemPendingRequests::await_response(Frames::SystemCall::Id id) {
		std::shared_ptr<Pending> pending;
		{
			std::lock_guard lock{mtx};
			const auto      it = promises.find(id);
			if (it == promises.end()) {
				throw ClientSideError(std::format("Invalid system request id {}", id));
			}
			pending = it->second;
		}
		if (pending->failure) {
			return std::unexpected(*pending->failure);
		}
		if (pending->future.wait_for(std::chrono::seconds(RESPONSE_WAITING_TIME_OUT_SECONDS))
			== std::future_status::timeout) {
			std::lock_guard lock{mtx};
			promises.erase(id);
			return std::unexpected(RequestFailure::CANCEL);
		}
		std::optional<SystemOutput> response;
		try {
			response.emplace(pending->future.get());
		} catch (const std::exception &) {
			std::lock_guard lock{mtx};
			promises.erase(id);
			return std::unexpected(RequestFailure::DISCONNECTION);
		}
		std::lock_guard lock{mtx};
		promises.erase(id);
		return response;
	}

	void SystemPendingRequests::fail_all_by_disconnection() {
		std::lock_guard lock{mtx};
		for (auto &[id, pending] : promises) {
			pending->failure = RequestFailure::DISCONNECTION;
			pending->promise.set_exception(std::make_exception_ptr(std::runtime_error("Client disconnected")));
		}
	}
} // namespace Karkinolution::Networking::ClientSide::Protocol
