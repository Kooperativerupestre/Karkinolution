#pragma once
#include "karkinolution/networking/frame/errors.hpp"

#include <cstdint>

namespace Karkinolution {
	namespace Networking {
		namespace Frames::World {
			namespace Invariants {
				void constexpr size_after_declared_size_cannot_be_zero(std::size_t declared_size,
																	   std::size_t bytes_size) {
					if (declared_size == bytes_size) {
						throw FrameInvariantError("Payload size cannot be equal zero");
					}
				}

			} // namespace Invariants
		} // namespace Frames::World
	} // namespace Networking

} // namespace Karkinolution