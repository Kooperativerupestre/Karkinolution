#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/message_type_size.hpp"

#include <karkinolution/binary/frames/processor.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>

std::vector<std::byte> FrameProcessor::process(const ParsedFrame &frame, const World &world) {

	if (!std::holds_alternative<BinarySubTypes::Request>(frame.sub_type)) {
		throw BufferError("Server received a non-Request frame type");
	}
	const auto sub_type = std::get<BinarySubTypes::Request>(frame.sub_type);

	switch (sub_type) {
		case BinarySubTypes::Request::GET_CREATURE: {
			auto bytes = CreatureAPIBYA::get_creature(world, frame.payload);
			return bytes;
		}
	}
	std::unreachable();
}
