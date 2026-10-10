#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/core/error.hpp"

#include <chrono>
#include <karkinolution/networking/sdk/src/client/pending_request.hpp>
#include <karkinolution/networking/sdk/src/error.hpp>

using KarkinolutionSDK::Protocol::PendingRequests;

bool PendingRequests::add_request(RequestId id) {
	std::lock_guard lock{mtx};
	if (pendings.contains(id) || futures.contains(id)) {
		return false;
	}

	auto &p = pendings[id];
	futures.emplace(id, p.get_future());
	return true;
}

bool PendingRequests::add_response(const ResponseFrame &r) {
	std::promise<ResponseFrame> p;
	{
		std::lock_guard lock{mtx};
		auto            it = pendings.find(r.request_id);
		if (it == pendings.end()) {
			return false;
		}
		p = std::move(it->second);
		pendings.erase(it);
	}
	p.set_value(r);
	return true;
}

void PendingRequests::cancel(RequestId id) {
	std::lock_guard lock{mtx};
	pendings.erase(id);
}

ResponseFrame PendingRequests::get_or_throw(std::future<ResponseFrame> &fut) {
	try {
		return fut.get();
	} catch (const std::future_error &e) {
		if (e.code() == std::future_errc::broken_promise) {
			throw ClientError("Connection closed before response arrived");
		}
		throw;
	}
}

std::optional<ResponseFrame> PendingRequests::pop_response(RequestId id) {
	std::future<ResponseFrame> fut;

	{
		std::lock_guard lock{mtx};
		auto            it = futures.find(id);
		if (it == futures.end()) {
			throw SimulationError("Invalid access to PendingRequests");
		}

		if (it->second.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
			return std::nullopt;
		}

		fut = std::move(it->second);
		futures.erase(it);
	}
	return get_or_throw(fut);
}

std::optional<ResponseFrame> PendingRequests::await_response(RequestId id) {
	std::future<ResponseFrame> fut;

	{
		std::lock_guard lock{mtx};
		auto            it = futures.find(id);
		if (it == futures.end()) {
			throw SimulationError("Invalid access to PendingRequests");
		}

		fut = std::move(it->second);
		futures.erase(it);
	}

	if (fut.wait_for(std::chrono::seconds(RESPONSE_WAITING_TIME_OUT_SECONDS))
		== std::future_status::timeout) {
		cancel(id);
		return std::nullopt;
	}
	return get_or_throw(fut);
}