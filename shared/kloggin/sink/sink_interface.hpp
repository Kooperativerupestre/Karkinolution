#pragma once
#include <string>

namespace KShared {
	namespace KLoggin {
		namespace Sink {
			template <typename T>
			concept Sink = requires(T sink, const std::string &message) {
				{ sink.write(message) } -> std::same_as<void>;
			};
		} // namespace Sink
	} // namespace KLoggin
} // namespace KShared