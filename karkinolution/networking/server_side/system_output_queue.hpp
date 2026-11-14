#pragma once
#include "karkinolution/networking/frame/models.hpp"

#include <cstdint>
#include <queue>

namespace Karkinolution::Networking::ServerSide::Connection::ProtocolSessions {
	class SystemOutputQueue {
		private:

			static constexpr std::size_t                           SYSTEM_OUTPUT_MAX_SIZE = 20;
			std::queue<Frames::Models::SystemOutput::SystemOutput> frames;

		public:

			const auto &view() const {
				return frames;
			}

			bool add(const Frames::Models::SystemOutput::SystemOutput &frame) {
				if (frames.size() >= SYSTEM_OUTPUT_MAX_SIZE) {
					return false;
				}

				frames.emplace(frame);
				return true;
			}

			Frames::Models::SystemOutput::SystemOutput pop_front() {
				auto frame = std::move(frames.front());
				frames.pop();
				return frame;
			}
	};
} // namespace Karkinolution::Networking::ServerSide::Connection::ProtocolSessions
