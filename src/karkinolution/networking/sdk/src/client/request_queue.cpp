#include "karkinolution/core/error.hpp"

#include <karkinolution/networking/sdk/src/client/request_queue.hpp>

using KarkinolutionSDK::Protocol::RequestQueue;

bool RequestQueue::add(std::span<const std::byte> bytes) {
	std::lock_guard lock{mutex};

	if (queue.size() >= MAX_REQUEST_QUEUE_SIZE) {
		return false;
	}

	queue.emplace(bytes.begin(), bytes.end());
	return true;
}

std::optional<std::vector<std::byte>> RequestQueue::pop_front() {
	std::lock_guard lock{mutex};
	if (queue.empty()) {
		return std::nullopt;
	}

	auto front = std::move(queue.front());
	queue.pop();
	return front;
}