#pragma once

#include <karkinolution/networking/frame/world/interpreters/models/territory.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace API {
		namespace Territory {
			Karkinolution::Networking::Frames::World::Interpreters::Models::Territory::Territory
			see_territory(const Karkinolution::World::World &world);
		} // namespace Territory
	} // namespace API
} // namespace Karkinolution
