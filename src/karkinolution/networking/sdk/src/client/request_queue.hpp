#pragma once
#include <mutex>
#include <queue>
#include <vector>

// Mutex is used here to allow different threads using the same queue

namespace KarkinolutionSDK {
	namespace Protocol {
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

				// returns nullopt if the queue is empty
				std::optional<std::vector<std::byte>> pop_front();
		};
	} // namespace Protocol
} // namespace KarkinolutionSDK