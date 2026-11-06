#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/protocol/layouts.hpp"
#include "karkinolution/math/units.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/networking/sdk/src/interpreters/generic_id.hpp"
#include "karkinolution/world/world.hpp"

#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/test/utils.hpp>

Karkinolution::World::World TestUtils::make_small_world() {
	Karkinolution::World::WorldPreset preset{
		.seed    = 1,
		.size    = Karkinolution::Math::Units::Size{.width  = Karkinolution::Math::Units::Width{100.0},
													.height = Karkinolution::Math::Units::Height{100.0}},
		.epsilon = 5.0f};
	return {preset};
}

std::vector<std::byte> TestUtils::make_see_creature_frame(RequestId id, Karkinolution::Core::Id::BaseIdType creature_id)

{
	const auto payload = CreatureSRI::Request::make_see_creature(creature_id);
	return Karkinolution::Binary::Frames::Motor::make_request_raw_bytes(
		RequestSubTypes::SEE,
		id,
		Resource::CREATURE,
		std::vector<std::byte>(payload.begin(), payload.end()),
		Karkinolution::Binary::Frames::Models::Request::SeeExtra{
			.target = Karkinolution::Binary::Protocol::Request::SeeTarget::ONE});
}

KarkinolutionSDK::Client::RequestData
TestUtils::make_see_creature_request_data(Karkinolution::Core::Id::BaseIdType creature_id) {
	const auto payload = CreatureSRI::Request::make_see_creature(creature_id);
	const auto built   = Karkinolution::Binary::Frames::Motor::make_request(
        RequestSubTypes::SEE,
        KarkinolutionSDK::Interpreters::GENERIC_INTERPRETER_ID,
        Resource::CREATURE,
        std::vector<std::byte>(payload.begin(), payload.end()),
        Karkinolution::Binary::Frames::Models::Request::SeeExtra{
			  .target = Karkinolution::Binary::Protocol::Request::SeeTarget::ONE});
	return {.sub_type = built.get_sub_type(),
			.resource = built.get_resource(),
			.payload  = built.get_payload(),
			.extra    = built.get_extra()};
}
