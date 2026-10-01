#include "karkinolution/api/creature.hpp"
#include "karkinolution/binary/binary_validators.hpp"
#include "karkinolution/binary/deserialization/interpreters/creature.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/core/error.hpp"

#include <karkinolution/binary/frames/processor.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>

std::vector<std::byte> FrameProcessor::process(const ParsedFrame &frame, const World &world) {

	if (!std::holds_alternative<BinarySubTypes::Request>(frame.sub_type)) {
		throw BufferError("Server received a non-Request frame type");
	}
	const auto sub_type = std::get<BinarySubTypes::Request>(frame.sub_type);

	const auto resource = frame.resource.value();

	BinaryValidator::validate_request_sub_type_and_resource(static_cast<uint8_t>(sub_type),
															static_cast<uint8_t>(resource));


	if (sub_type == BinarySubTypes::Request::SEE) {
		if (resource == RequestResource::CREATURE) {
			return CreatureAPIBYA::get_creature(world, frame.payload);
		}
	}


	throw SimulationError(
		std::format("Unreachable: The validation before the process in the frame processor "
					"must treat this case: request sub type {} and request resource {}",
					static_cast<uint8_t>(sub_type),
					static_cast<uint8_t>(resource)));
}
