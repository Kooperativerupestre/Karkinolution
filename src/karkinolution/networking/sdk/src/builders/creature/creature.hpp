#pragma once
#include "karkinolution/core/id.hpp"

#include <karkinolution/networking/sdk/src/client/client.hpp>

namespace KarkinolutionSDK {
	namespace Builders {
		namespace Creature {
			KarkinolutionSDK::Client::ClientRequestData see_creature(BaseIdType id);
		} // namespace Creature
	} // namespace Builders
} // namespace KarkinolutionSDK