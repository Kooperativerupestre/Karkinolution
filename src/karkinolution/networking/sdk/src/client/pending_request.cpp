#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/core/error.hpp"

#include <algorithm>
#include <chrono>
#include <karkinolution/networking/sdk/src/client/pending_request.hpp>
#include <karkinolution/networking/sdk/src/error.hpp>

using KarkinolutionSDK::Protocol::FutureRequest;
using KarkinolutionSDK::Protocol::PendingRequests;
using KarkinolutionSDK::Protocol::RequestFailure;
using KarkinolutionSDK::Protocol::ResponseResult;

FutureRequest::FutureRequest()
	: promise(std::in_place)
	, fut_response(promise.value().get_future()) {}

bool FutureRequest::is_ready_unlocked() const {
	return fut_response.valid()
		&& fut_response.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
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
	{
		std::lock_guard lock{mtx};

		if (failure.has_value()) {
			return std::unexpected{*failure};
		}

		if (wait_in_progress) {
			throw std::logic_error("A wait is already in progress");
		}

		if (!fut_response.valid()) {
			return std::optional<ResponseFrameVariant>{};
		}

		wait_in_progress = true;
	}

	std::future_status status;

	try {
		status = fut_response.wait_for(timeout);
	} catch (...) {
		std::lock_guard lock{mtx};
		wait_in_progress = false;
		throw;
	}

	if (status == std::future_status::timeout) {
		fail(RequestFailure::CANCEL);
	}

	std::lock_guard lock{mtx};

	wait_in_progress = false;

	if (failure.has_value()) {
		return std::unexpected{*failure};
	}

	if (!fut_response.valid()) {
		return std::optional<ResponseFrameVariant>{};
	}

	return std::optional<ResponseFrameVariant>{fut_response.get()};
}

bool PendingRequests::erase_request(RequestId id, const std::shared_ptr<FutureRequest> expected) {
	std::lock_guard lock{mtx};
	auto            it = pendings.find(id);

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
				return frame.request_id;
			},
			response);
		auto it = pendings.find(request_id);
		if (it == pendings.end()) {
			throw CPPSdkError(
				std::format("Invalid try to add a response to an inexistent request with id = {}",
							request_id));
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
			throw CPPSdkError(
				std::format("Invalid pop response of an inexistent request with id = {}", id));
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
			throw CPPSdkError(
				std::format("Invalid await response of a nonexistent request with id = {}", id));
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