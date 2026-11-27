#pragma once

#include "shared/core/id.hpp"

#include <array>
#include <vector>

namespace Karkinolution::Networking::Frames::World::MessageInterpreters {
	namespace SRI::Soil::Request {
		std::array<std::byte, 8> make_see_soil(KShared::Core::Id::BaseIdType id);
		std::array<std::byte, 1> make_see_all_soils();
	} // namespace SRI::Soil::Request

	namespace DSI::Soil::Request {
		KShared::Core::Id::BaseIdType interpret_like_get_soil(const std::vector<std::byte> &payload);
	} // namespace DSI::Soil::Request
} // namespace Karkinolution::Networking::Frames::World::MessageInterpreters

namespace SoilMessageSRI = Karkinolution::Networking::Frames::World::MessageInterpreters::SRI::Soil::Request;
namespace SoilMessageDSI = Karkinolution::Networking::Frames::World::MessageInterpreters::DSI::Soil::Request;
