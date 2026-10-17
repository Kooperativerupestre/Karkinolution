#include "karkinolution/networking/sdk/src/builders/corpse/corpse.hpp"

#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/serialization/interpreters/corpse/corpse.hpp"
#include "karkinolution/networking/sdk/src/builders/generic_id.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"

#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/binary/serialization/interpreters/corpse/alias.hpp>
#include <karkinolution/networking/sdk/src/builders/corpse/alias.hpp>

ClientRequestData CorpseSDK::see_corpse(BaseIdType id) {
	const auto payload = CorpseSRI::Request::make_see_corpse(id);

	const auto built =
		Frames::Motor::make_request(RequestSubTypes::SEE,
									GENERIC_BUILDER_ID,
									RequestResource::CORPSE,
									std::vector<std::byte>(payload.begin(), payload.end()));

	return ClientRequestData{.sub_type = built.sub_type,
							 .resource = built.resource,
							 .payload  = built.payload};
}