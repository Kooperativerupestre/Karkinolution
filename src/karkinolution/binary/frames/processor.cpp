#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/message_type_size.hpp"

#include <karkinolution/binary/frames/processor.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>

void FrameProcessor::add(const ParsedFrame &frame) {
	assert(frame.type == BinaryTypes::Request);
	frames.push_back(frame);
}

std::vector<std::byte> FrameProcessor::process(const World &world) {
	const auto &frame = frames.front();

	const auto sub_type = std::get<BinarySubTypes::Request>(frame.sub_type);

	switch (sub_type) {
		case BinarySubTypes::Request::GET_CREATURE: {
			auto bytes = CreatureAPIBYA::get_creature(world, frame.payload);
			frames.pop_front();
			return bytes;
		}
	}
	std::unreachable();
}