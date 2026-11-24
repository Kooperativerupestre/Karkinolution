#include "karkinolution/api/creature.hpp"
#include "karkinolution/networking/frame/world/binary_validators.hpp"
#include "karkinolution/networking/frame/world/contract.hpp"
#include "karkinolution/networking/frame/world/core_aliases.hpp"
#include "karkinolution/networking/frame/world/models.hpp"
#include "karkinolution/networking/frame/world/utils.hpp"
#include "karkinolution/networking/server_side/api_adapter/corpse.hpp"

#include <format>
#include <karkinolution/networking/frame/world/interpreters/creature.hpp>
#include <karkinolution/networking/frame/world/processor.hpp>
#include <karkinolution/networking/server_side/api_adapter/creature.hpp>
#include <karkinolution/networking/server_side/api_adapter/soil.hpp>
#include <karkinolution/networking/server_side/api_adapter/territory.hpp>

namespace Karkinolution::Networking::Frames::World::Processor {
	ResponseFrame process(const RequestFrame &frame, Karkinolution::World::World &world) {
		const auto sub_type = frame.get_sub_type();
		const auto resource = frame.get_resource();

		if (sub_type == RequestSubTypes::SEE) {
			const auto extra = std::get<Models::Request::SeeExtra>(frame.get_extra());
			if (resource == Resource::CREATURE) {
				return Networking::ServerSide::APIBYA::Creature::see_creature(world,
																			  frame.get_id(),
																			  frame.get_payload());
			} else if (resource == Resource::CORPSE) {
				return Networking::ServerSide::APIBYA::Corpse::see_corpse(world, frame.get_id(), frame.get_payload());
			} else if (resource == Resource::SOIL) {
				if (extra.target == Request::SeeTarget::ONE) {
					return Networking::ServerSide::APIBYA::Soil::see_soil(world, frame.get_id(), frame.get_payload());
				} else {
					return Networking::ServerSide::APIBYA::Soil::see_all_soils(world,
																			   frame.get_id(),
																			   frame.get_payload());
				}
			} else if (resource == Resource::TERRITORY) {
				return Networking::ServerSide::APIBYA::Territory::see_territory(world, frame.get_id());
			}
		} else if (sub_type == RequestSubTypes::CREATE) {
			if (resource == Resource::CREATURE) {
				return Networking::ServerSide::APIBYA::Creature::create_creature(world,
																				 frame.get_id(),
																				 frame.get_payload());
			} else if (resource == Resource::CORPSE) {
				return Networking::ServerSide::APIBYA::Corpse::erase_corpse(world, frame.get_id(), frame.get_payload());
			} else if (resource == Resource::SOIL) {
				return Networking::ServerSide::APIBYA::Soil::erase_soil(world, frame.get_id(), frame.get_payload());
			}
		} else if (sub_type == RequestSubTypes::CYCLE) {
			if (resource == Resource::CREATURE) {
				return Networking::ServerSide::APIBYA::Creature::tick_creature(world,
																			   frame.get_id(),
																			   frame.get_payload());
			} else if (resource == Resource::CORPSE) {
				return Networking::ServerSide::APIBYA::Corpse::tick_corpse(world, frame.get_id(), frame.get_payload());
			} else if (resource == Resource::SOIL) {
				return Networking::ServerSide::APIBYA::Soil::tick_soil(world, frame.get_id(), frame.get_payload());
			}
		} else if (sub_type == RequestSubTypes::ERASE) {
			if (resource == Resource::CREATURE) {
				return Networking::ServerSide::APIBYA::Creature::erase_creature(world,
																				frame.get_id(),
																				frame.get_payload());
			}
		}

		KShared::Functional::unreachable_or_throw(
			std::format("Non implemented yey: request sub type {} and request resource {}",
						Karkinolution::Networking::Frames::World::Utils::cast_numeric(sub_type),
						Karkinolution::Networking::Frames::World::Utils::cast_numeric(resource)));
	}
} // namespace Karkinolution::Networking::Frames::World::Processor
