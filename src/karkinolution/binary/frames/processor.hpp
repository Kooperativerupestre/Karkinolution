#pragma once
#include "karkinolution/world/world.hpp"

#include <karkinolution/binary/frames/parser.hpp>

namespace FrameProcessor {
	std::vector<std::byte> process(const ParsedFrame &frame, const World &world);
} // namespace FrameProcessor
