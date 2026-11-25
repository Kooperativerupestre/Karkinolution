#pragma once

#include "karkinolution/networking/client_side/client/client.hpp"
#include "shared/core/id.hpp"

#include <karkinolution/networking/frame/world/core_aliases.hpp>
#include <karkinolution/networking/frame/world/motor.hpp>

namespace Karkinolution::Testing::Frame {

	std::vector<std::byte> make_see_creature_frame(Karkinolution::Networking::Frames::World::RequestId id,
												   KShared::Core::Id::BaseIdType                       creature_id);

	Karkinolution::Networking::ClientSide::Client::WorldRequestData
	make_see_creature_request_data(KShared::Core::Id::BaseIdType creature_id);

} // namespace Karkinolution::Testing::Frame
