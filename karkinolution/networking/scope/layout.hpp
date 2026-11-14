#pragma once
#include <cstdint>

namespace Karkinolution {
	namespace Networking {
		namespace Scope {
			enum class Scope : uint8_t {
				WORLD  = 1,
				SERVER = 0
			};

		} // namespace Scope
	} // namespace Networking
} // namespace Karkinolution