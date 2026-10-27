#include "karkinolution/api/creature.hpp"
#include "karkinolution/binary/binary_validators.hpp"
#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/frames/models.hpp"
#include "karkinolution/binary/frames/parser.hpp"
#include "karkinolution/binary/protocol/contract.hpp"
#include "karkinolution/core/error.hpp"
#include "karkinolution/networking/api_adapter/corpse.hpp"

#include <karkinolution/binary/frames/processor.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/networking/api_adapter/creature.hpp>
#include <karkinolution/networking/api_adapter/soil.hpp>

ResponseFrame Karkinolution::Binary::Frames::Processor::process(const RequestFrame &frame,
																World              &world) {

	const auto sub_type = frame.sub_type;

	const auto resource = frame.resource;


	if (sub_type == RequestSubTypes::SEE) {
		const auto extra = std::get<Models::Request::SeeExtra>(frame.extra);
		if (resource == Resource::CREATURE) {
			return CreatureAPIBYA::see_creature(world, frame.id, frame.payload);
		} else if (resource == Resource::CORPSE) {
			return CorpseAPIBYA::see_corpse(world, frame.id, frame.payload);
		} else if (resource == Resource::SOIL) {
			if (extra.target == Protocol::Request::SeeTarget::ONE) {
				return Karkinolution::Networking::APIBYA::Soil::see_soil(world,
																		 frame.id,
																		 frame.payload);
			} else {
				return Karkinolution::Networking::APIBYA::Soil::see_all_soils(world,
																			  frame.id,
																			  frame.payload);
			}
		}
	} else if (sub_type == RequestSubTypes::CREATE) {
		if (resource == Resource::CREATURE) {
			return CreatureAPIBYA::create_creature(world, frame.id, frame.payload);
		}
	}


	Karkinolution::Functional::unreachable_or_throw(
		std::format("Non implemented yey: request sub type {} and request resource {}",
					Binary::Utils::cast_numeric(sub_type),
					Binary::Utils::cast_numeric(resource)));
}
