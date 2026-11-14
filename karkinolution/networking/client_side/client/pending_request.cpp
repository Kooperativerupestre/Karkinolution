#include "karkinolution/networking/client_side/client/pending_request.hpp"

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "shared/core/error.hpp"

#include <algorithm>
#include <chrono>
#include <format>
#include <stdexcept>

namespace Karkinolution::Networking::ClientSide::Protocol {
	FutureRequest::FutureRequest()
		: promise(std::in_place)
		, fut_response(promise.value().get_future()) {}

	bool FutureRequest::is_ready_unlocked() const {
		return fut_response.valid() && fut_response.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
	}

	bool FutureRequest::set_response(const ResponseFrameVariant &response) {
		std::lock_guard lock{mtx};

		if (failure.has_value() || !promise.has_value()) {
			return false;
		}

		promise->set_value(response);
		promise.reset();
		return true;
	}

	bool FutureRequest::has_response() {
		std::lock_guard lock{mtx};
		return !failure.has_value() && !wait_in_progress && is_ready_unlocked();
	}

	bool FutureRequest::fail(RequestFailure type) {
		std::lock_guard lock{mtx};

		if (failure.has_value() || !promise.has_value()) {
			return false;
		}
		failure = type;
		promise.reset();
		return true;
	}

	ResponseResult FutureRequest::try_get() {
		std::lock_guard lock{mtx};

		if (failure.has_value()) {
			return std::unexpected(failure.value());
		}

		if (wait_in_progress || failure.has_value() || !is_ready_unlocked()) {
			return std::optional<ResponseFrameVariant>{};
		}

		return std::optional<ResponseFrameVariant>{fut_response.get()};
	}

	ResponseResult FutureRequest::await_response(std::chrono::seconds timeout) {
		std::future<ResponseFrameVariant> fut;
		{
			std::lock_guard lock{mtx};

			if (wait_in_progress) {
				return std::optional<ResponseFrameVariant>{};
			}

			if (failure.has_value()) {
				return std::unexpected(failure.value());
			}

			if (is_ready_unlocked()) {
				return std::optional<ResponseFrameVariant>{fut_response.get()};
			}

			wait_in_progress = true;
			fut              = std::move(fut_response);
		}

		const auto status = fut.wait_for(timeout);

		std::lock_guard lock{mtx};
		wait_in_progress = false;

		if (status == std::future_status::ready) {
			if (failure.has_value()) {
				return std::unexpected(failure.value());
			}
			try {
				return std::optional<ResponseFrameVariant>{fut.get()};
			} catch (const std::exception &) {
				return std::unexpected(RequestFailure::DISCONNECTION);
			}
		}

		fut_response = std::move(fut);
		failure      = RequestFailure::CANCEL;
		promise.reset();
		return std::unexpected(RequestFailure::CANCEL);
	}

	std::optional<RequestFailure> FutureRequest::get_failure() {
		std::lock_guard lock{mtx};
		return failure;
	}

	bool PendingRequests::erase_request(RequestId id, const std::shared_ptr<FutureRequest> expected) {
		std::lock_guard lock{mtx};
		const auto      it = pendings.find(id);
		if (it != pendings.end() && it->second == expected) {
			pendings.erase(it);
			return true;
		}
		return false;
	}

	bool PendingRequests::add_request(RequestId id) {
		auto request = std::make_shared<FutureRequest>();

		std::lock_guard lock{mtx};
		return pendings.emplace(id, std::move(request)).second;
	}

	bool PendingRequests::add_response(const ResponseFrameVariant &response) {
		std::shared_ptr<FutureRequest> request;
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
			request = it->second;
		}

		return request->set_response(response);
	}

	ResponseResult PendingRequests::pop_response(RequestId id) {
		std::shared_ptr<FutureRequest> request;
		{
			std::lock_guard lock{mtx};
			auto            it = pendings.find(id);
			if (it == pendings.end()) {
				throw ClientSideError(std::format("Invalid pop response of an inexistent request with id = {}", id));
			}
			request = it->second;
		}
		auto response = request->try_get();

		if (!response || response->has_value()) {
			erase_request(id, request);
		}

		return response;
	}

	ResponseResult PendingRequests::await_response(RequestId id) {
		std::shared_ptr<FutureRequest> request;

		{
			std::lock_guard lock{mtx};

			auto it = pendings.find(id);

			if (it == pendings.end()) {
				throw ClientSideError(std::format("Invalid await response of a nonexistent request with id = {}", id));
			}

			request = it->second;
		}

		auto result = request->await_response(std::chrono::seconds(RESPONSE_WAITING_TIME_OUT_SECONDS));

		erase_request(id, request);

		return result;
	}

	void PendingRequests::fail_all_by_disconnection() {
		std::vector<std::shared_ptr<FutureRequest>> requests;

		{
			std::lock_guard lock{mtx};

			requests.reserve(pendings.size());

			for (const auto &[id, request] : pendings) {
				requests.push_back(request);
			}
		}

		for (const auto &request : requests) {
			request->fail(RequestFailure::DISCONNECTION);
		}
	}

	bool SystemPendingRequests::add_request(RequestId id) {
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

	SystemResponseResult SystemPendingRequests::await_response(RequestId id) {
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
