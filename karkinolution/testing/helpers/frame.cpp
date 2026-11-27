#include "karkinolution/testing/helpers/frame.hpp"

#include "karkinolution/networking/client_side/interpreters/generic_id.hpp"
#include "karkinolution/networking/frame/world/interpreters/creature.hpp"
#include "karkinolution/networking/frame/world/message_interpreters/creature.hpp"

std::vector<std::byte>
Karkinolution::Testing::Frame::make_see_creature_frame(Karkinolution::Networking::Frames::World::RequestId id,
													   KShared::Core::Id::BaseIdType creature_id) {
	const auto payload = CreatureMessageSRI::make_see_creature(creature_id);
	return Karkinolution::Networking::Frames::World::Motor::make_request_raw_bytes(
		Karkinolution::Networking::Frames::World::RequestSubTypes::SEE,
		id,
		Karkinolution::Networking::Frames::World::Resource::CREATURE,
		std::vector<std::byte>(payload.begin(), payload.end()),
		Karkinolution::Networking::Frames::World::Models::Request::SeeExtra{
			.target = Karkinolution::Networking::Frames::World::Request::SeeTarget::ONE});
}

Karkinolution::Networking::ClientSide::Client::WorldRequestData
Karkinolution::Testing::Frame::make_see_creature_request_data(KShared::Core::Id::BaseIdType creature_id) {
	const auto payload = CreatureMessageSRI::make_see_creature(creature_id);
	const auto built   = Karkinolution::Networking::Frames::World::Motor::make_request(
        Karkinolution::Networking::Frames::World::RequestSubTypes::SEE,
        Karkinolution::Networking::ClientSide::Interpreters::GENERIC_INTERPRETER_ID,
        Karkinolution::Networking::Frames::World::Resource::CREATURE,
        std::vector<std::byte>(payload.begin(), payload.end()),
        Karkinolution::Networking::Frames::World::Models::Request::SeeExtra{
			  .target = Karkinolution::Networking::Frames::World::Request::SeeTarget::ONE});
	return {.sub_type = built.get_sub_type(),
			.resource = built.get_resource(),
			.payload  = built.get_payload(),
			.extra    = built.get_extra()};
}
