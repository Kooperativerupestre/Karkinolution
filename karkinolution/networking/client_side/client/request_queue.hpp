#pragma once

#include <mutex>
#include <optional>
#include <queue>
#include <span>
#include <vector>

namespace Karkinolution::Networking::ClientSide::Protocol {
	inline constexpr std::size_t MAX_REQUEST_QUEUE_SIZE = 200;

	class RequestQueue {
		private:

			mutable std::mutex                 mutex;
			std::queue<std::vector<std::byte>> queue;

		public:

			bool constexpr empty() const {
				std::lock_guard lock{mutex};
				return queue.empty();
			}

			bool add(std::span<const std::byte> bytes);

			std::optional<std::vector<std::byte>> pop_front();
	};
} // namespace Karkinolution::Networking::ClientSide::Protocol
