#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace APIBYA {
			namespace Territory {
				ResponseFrame see_territory(const World &world, RequestId request_id);
			} // namespace Territory
		} // namespace APIBYA
	} // namespace Networking
} // namespace Karkinolution
