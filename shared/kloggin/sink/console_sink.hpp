#pragma once

#include "shared/kloggin/sink/sink_interface.hpp"

#include <cstdint>
#include <iostream>
#include <string>
#include <utility>

namespace KShared {
	namespace KLoggin::Sink {

		enum class ConsoleSinkOutput : uint8_t {
			STDOUT,
			STDERR
		};

		template <ConsoleSinkOutput T> class ConsoleSink {
			public:

				void write(const std::string &message) {
					if constexpr (T == ConsoleSinkOutput::STDERR) {
						std::cerr << message << "\n";
						return;
					} else if constexpr (T == ConsoleSinkOutput::STDOUT) {
						std::cout << message << "\n";
						return;
					}
					std::unreachable();
				}
		};

		static_assert(Sink<ConsoleSink<ConsoleSinkOutput::STDOUT>>);
		static_assert(Sink<ConsoleSink<ConsoleSinkOutput::STDERR>>);
	} // namespace KLoggin::Sink
} // namespace KShared