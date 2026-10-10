#pragma once
#include "karkinolution/core/error.hpp"

#include <cstdint>

namespace FramesInvariants {
	void constexpr payload_size_cannot_be_zero(std::size_t declared_size, std::size_t bytes_size) {
		if (declared_size == bytes_size) {
			throw FrameInvariantError("Payload size cannot be equal zero");
		}
	}
} // namespace FramesInvariants