#pragma once

#include "shared/kloggin/levels.hpp"
#include "shared/kloggin/sink/sink_interface.hpp"

#include <mutex>
#include <sstream>
#include <string>

namespace KShared {
	namespace KLoggin {
		template <Sink::Sink T> class Logger {
			private:

				Level treshold;

				T                  sink;
				mutable std::mutex mutex;

				template <Level L> std::string make_message(const std::string &message) {
					std::ostringstream stream;
					stream << '[' << level_to_string<L>() << "] - " << message << '\n';
					return stream.str();
				}

				template <Level L> bool is_showing_unlocked_() const {
					return L <= treshold;
				}


			public:


				Logger(Level treshold, T &&sink)
					: treshold(treshold)
					, sink(std::move(sink)) {}

				Logger(T &&sink)
					: treshold(Level::INFO)
					, sink(std::move(sink)) {}

				template <Level L> bool is_showing() const {
					std::lock_guard lock{mutex};
					return is_showing_unlocked_<L>();
				}

				template <Level L> void set() {
					std::lock_guard lock{mutex};
					treshold = L;
				}

				template <Level L> void log(const std::string &message) {
					std::lock_guard lock{mutex};
					if (is_showing_unlocked_<L>()) {
						sink.write(message);
					}
				}
		};
	} // namespace KLoggin
} // namespace KShared