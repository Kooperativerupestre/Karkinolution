#pragma once
#include "karkinolution/binary/frames/parser.hpp"

#include <karkinolution/binary/frames/aliases.hpp>
#include <karkinolution/core/error.hpp>
#include <queue>
#include <vector>
inline constexpr size_t RESPONSE_QUEUE_MAX_SIZE = 200;

class ResponseQueue {
	private:

		std::queue<ResponseFrame> frames;

	public:

		const auto &view() const {
			return frames;
		}

		bool add(ResponseFrame frame) {
			if (frames.size() >= RESPONSE_QUEUE_MAX_SIZE) {
				return false;
			}

			frames.emplace(std::move(frame));
			return true;
		}

		ResponseFrame pop_front() {
			auto frame = std::move(frames.front());
			frames.pop();
			return frame;
		}
};