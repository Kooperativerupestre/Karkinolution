#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/view/client/client.hpp"

#include <karkinolution/binary/interpreters/corpse.hpp>

namespace KarkinolutionView {
	namespace World {
		namespace Queriers {
			namespace Corpse {
				std::optional<CorpseDSI::Models::Corpse>
				see_corpse(BaseIdType id, KarkinolutionView::Client::Client &client);
			} // namespace Corpse
		} // namespace Queriers
	} // namespace World
} // namespace KarkinolutionView