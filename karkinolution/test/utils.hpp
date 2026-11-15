#pragma once
#include "karkinolution/networking/client_side/client/client.hpp"
#include "shared/core/id.hpp"

#include <karkinolution/binary/frames/motor.hpp>
#include <karkinolution/world/world.hpp>

// kshared-only test utilities (random_string, append_to_buffer, add_trash)
#include <tests/kshared/test_utils.hpp>

namespace TestUtils {

	Karkinolution::World::World make_small_world();

	std::vector<std::byte> make_see_creature_frame(Karkinolution::Binary::RequestId id,
												   KShared::Core::Id::BaseIdType    creature_id);
	Karkinolution::Networking::ClientSide::Client::RequestData
	make_see_creature_request_data(KShared::Core::Id::BaseIdType creature_id);

} // namespace TestUtils
