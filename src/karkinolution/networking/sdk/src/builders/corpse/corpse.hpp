#pragma once

#include <karkinolution/core/id.hpp>
#include <karkinolution/networking/sdk/src/client/client.hpp>

namespace KarkinolutionSDK {
	namespace Builders {
		namespace Corpse {
			KarkinolutionSDK::Client::ClientRequestData see_corpse(BaseIdType id);
		} // namespace Corpse
	} // namespace Builders
} // namespace KarkinolutionSDK