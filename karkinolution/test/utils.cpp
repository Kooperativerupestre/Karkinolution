#include "karkinolution/binary/core_aliases.hpp"
#include "karkinolution/binary/protocol/layouts.hpp"
#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/networking/client_side/interpreters/generic_id.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/math/units.hpp"

#include <karkinolution/binary/interpreters/creature.hpp>
#include <karkinolution/test/utils.hpp>
#include <shared/functional/k_random.hpp>

Karkinolution::World::World TestUtils::make_small_world() {
	Karkinolution::World::WorldPreset preset{
		.seed    = 1,
		.size    = KShared::Math::Units::Size{.width  = KShared::Math::Units::Width{100.0},
											  .height = KShared::Math::Units::Height{100.0}},
		.epsilon = 5.0f};
	return {preset};
}

std::vector<std::byte> TestUtils::make_see_creature_frame(Karkinolution::Binary::RequestId id,
														  KShared::Core::Id::BaseIdType    creature_id) {
	const auto payload = CreatureSRI::Request::make_see_creature(creature_id);
	return Karkinolution::Binary::Frames::Motor::make_request_raw_bytes(
		Karkinolution::Binary::RequestSubTypes::SEE,
		id,
		Karkinolution::Binary::Resource::CREATURE,
		std::vector<std::byte>(payload.begin(), payload.end()),
		Karkinolution::Binary::Frames::Models::Request::SeeExtra{
			.target = Karkinolution::Binary::Protocol::Request::SeeTarget::ONE});
}

Karkinolution::Networking::ClientSide::Client::RequestData
TestUtils::make_see_creature_request_data(KShared::Core::Id::BaseIdType creature_id) {
	const auto payload = CreatureSRI::Request::make_see_creature(creature_id);
	const auto built   = Karkinolution::Binary::Frames::Motor::make_request(
        Karkinolution::Binary::RequestSubTypes::SEE,
        Karkinolution::Networking::ClientSide::Interpreters::GENERIC_INTERPRETER_ID,
        Karkinolution::Binary::Resource::CREATURE,
        std::vector<std::byte>(payload.begin(), payload.end()),
        Karkinolution::Binary::Frames::Models::Request::SeeExtra{
			  .target = Karkinolution::Binary::Protocol::Request::SeeTarget::ONE});
	return {.sub_type = built.get_sub_type(),
			.resource = built.get_resource(),
			.payload  = built.get_payload(),
			.extra    = built.get_extra()};
}
