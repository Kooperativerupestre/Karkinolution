#include "karkinolution/api/creature.hpp"
#include "karkinolution/binary/binary_validators.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/networking/api_adapter/corpse.hpp"

#include <karkinolution/binary/frames/processor.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>

ResponseFrame Frames::Processor::process(const RequestFrame &frame, const World &world) {

	const auto sub_type = frame.sub_type;

	const auto resource = frame.resource;

	BinaryValidator::Request::validate_request_sub_type_and_resource(
		static_cast<uint8_t>(sub_type),
		static_cast<uint8_t>(resource));


	if (sub_type == Frames::Types::BinarySubTypes::Request::SEE) {
		if (resource == Frames::Types::RequestResource::CREATURE) {
			return CreatureAPIBYA::see_creature(world, frame.id, frame.payload);
		} else if (resource == Frames::Types::RequestResource::CORPSE) {
			return CorpseAPIBYA::see_corpse(world, frame.id, frame.payload);
		}
	}


	throw SimulationError(
		std::format("Unreachable: The validation before the process in the frame processor "
					"must treat this case: request sub type {} and request resource {}",
					static_cast<Frames::Types::FrameSubType>(sub_type),
					static_cast<Frames::Types::RequestResourceEnumType>(resource)));
}
