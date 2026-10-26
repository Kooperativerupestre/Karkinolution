#pragma once

#include "karkinolution/binary/frames/aliases.hpp"
#include "karkinolution/binary/protocol/aliases.hpp"

#include <karkinolution/world/world.hpp>

namespace Karkinolution {
	namespace Networking {
		namespace APIBYA {
			namespace Soil {
				ResponseFrame see_soil(const World                  &world,
									   RequestId                     request_id,
									   const std::vector<std::byte> &payload);
			} // namespace Soil
		} // namespace APIBYA
	} // namespace Networking
} // namespace Karkinolution