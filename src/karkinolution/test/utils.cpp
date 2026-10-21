#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/message_type_size.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/networking/sdk/src/interpreters/generic_id.hpp"

#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/test/utils.hpp>

World TestUtils::make_small_world() {
	WorldPreset preset{
		.seed    = 1,
		.size    = Size{.lateral = Lateral{100.0}, .height = Height{100.0}, .depth = Depth{100.0}},
		.epsilon = 5.0f};
	return WorldFactory::create_world(preset);
}

std::vector<std::byte> TestUtils::make_see_creature_frame(Frames::Types::RequestId id,
														  BaseIdType               creature_id)

{
	const auto payload = CreatureSRI::Request::make_see_creature(creature_id);
	return Frames::Motor::make_request_raw_bytes(
		RequestSubTypes::SEE,
		id,
		RequestResource::CREATURE,
		std::vector<std::byte>(payload.begin(), payload.end()));
}

KarkinolutionSDK::Client::ClientRequestData
TestUtils::make_see_creature_request_data(BaseIdType creature_id) {
	const auto payload = CreatureSRI::Request::make_see_creature(creature_id);
	const auto built =
		Frames::Motor::make_request(RequestSubTypes::SEE,
									GENERIC_INTERPRETER_ID,
									RequestResource::CREATURE,
									std::vector<std::byte>(payload.begin(), payload.end()));
	return {.sub_type = built.sub_type, .resource = built.resource, .payload = built.payload};
}