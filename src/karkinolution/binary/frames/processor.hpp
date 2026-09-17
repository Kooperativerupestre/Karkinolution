#pragma once
#include "karkinolution/world/world.hpp"

#include <karkinolution/binary/frames/parser.hpp>

class FrameProcessor {
	private:

		std::deque<ParsedFrame> frames;

	public:


		// a frame with type different from Request isn't allowed here

		const std::deque<ParsedFrame> &view() const {
			return frames;
		}

		void                   add(const ParsedFrame &frame);
		std::vector<std::byte> process(const World &world);
};