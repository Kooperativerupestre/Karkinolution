#pragma once

#include <array>

namespace Karkinolution::Networking::Frames::World::MessageInterpreters::SRI::Territory::Request {
	std::array<std::byte, 0> make_see_territory();
} // namespace Karkinolution::Networking::Frames::World::MessageInterpreters::SRI::Territory::Request

namespace TerritoryMessageSRI = Karkinolution::Networking::Frames::World::MessageInterpreters::SRI::Territory::Request;
