#pragma once
#include "karkinolution/core/error.hpp"

#include <cstdint>

namespace Karkinolution {
	namespace Binary {
		namespace Frames {
			namespace Invariants {
				void constexpr size_after_declared_size_cannot_be_zero(std::size_t declared_size,
																	   std::size_t bytes_size) {
					if (declared_size == bytes_size) {
						throw FrameInvariantError("Payload size cannot be equal zero");
					}
				}

			} // namespace Invariants
		} // namespace Frames
	} // namespace Binary

} // namespace Karkinolution