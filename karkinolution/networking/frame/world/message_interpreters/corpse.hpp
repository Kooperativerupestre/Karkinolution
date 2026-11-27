#pragma once

#include "shared/core/id.hpp"

#include <array>
#include <vector>

namespace Karkinolution::Networking::Frames::World::MessageInterpreters {
	namespace SRI::Corpse::Request {
		std::array<std::byte, 8> make_see_corpse(KShared::Core::Id::BaseIdType id);
		std::array<std::byte, 1> make_see_all_corpses();
	} // namespace SRI::Corpse::Request

	namespace DSI::Corpse::Request {
		KShared::Core::Id::BaseIdType interpret_like_see_corpse(const std::vector<std::byte> &payload);
	} // namespace DSI::Corpse::Request
} // namespace Karkinolution::Networking::Frames::World::MessageInterpreters

namespace CorpseMessageSRI = Karkinolution::Networking::Frames::World::MessageInterpreters::SRI::Corpse::Request;
namespace CorpseMessageDSI = Karkinolution::Networking::Frames::World::MessageInterpreters::DSI::Corpse::Request;
