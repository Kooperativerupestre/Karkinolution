#pragma once

#include <karkinolution/binary/interpreters/models/territory.hpp>
#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace API {
		namespace Territory {
			Binary::Interpreters::Models::Territory::Territory see_territory(const World &world);
		} // namespace Territory
	} // namespace API
} // namespace Karkinolution
