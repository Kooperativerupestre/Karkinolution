#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace KShared {
	namespace KLoggin {

		enum class Level : uint8_t {
			TRACE   = 0,
			DEBUG   = 1,
			INFO    = 2,
			WARNING = 3,
			ERROR   = 4
		};

		template <Level L> std::string level_to_string() {
			switch (L) {
				case Level::TRACE:
					return "TRACE";
				case Level::DEBUG:
					return "DEBUG";
				case Level::ERROR:
					return "ERROR";
				case Level::WARNING:
					return "WARNING";
				case Level::INFO:
					return "INFO";
				default:
					std::unreachable();
			}
		}
	} // namespace KLoggin
} // namespace KShared