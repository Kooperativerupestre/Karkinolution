#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/networking/sdk/src/builders/generic_id.hpp"

#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/networking/sdk/src/builders/creature/alias.hpp>
#include <karkinolution/networking/sdk/src/builders/creature/creature.hpp>
#include <karkinolution/networking/sdk/src/client/alias.hpp>

ClientRequestData CreatureSDK::see_creature(BaseIdType id) {

	const auto payload = CreatureSRI::Request::make_see_creature(id);

	const auto built =
		Frames::Motor::make_request(RequestSubTypes::SEE,
									GENERIC_BUILDER_ID,
									RequestResource::CREATURE,
									std::vector<std::byte>(payload.begin(), payload.end()));

	return ClientRequestData{.sub_type = built.sub_type,
							 .resource = built.resource,
							 .payload  = built.payload};
}
