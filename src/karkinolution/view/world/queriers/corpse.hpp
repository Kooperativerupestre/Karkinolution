#pragma once
#include "karkinolution/view/client/client.hpp"
#include "shared/core/id.hpp"

#include <karkinolution/binary/interpreters/models/corpse.hpp>

namespace KarkinolutionView {
	namespace World {
		namespace Queriers {
			namespace Corpse {
				std::optional<Karkinolution::Binary::Interpreters::Models::Corpse::Corpse>
				see_corpse(KShared::Core::Id::BaseIdType id, KarkinolutionView::Client::Client &client);
			} // namespace Corpse
		} // namespace Queriers
	} // namespace World
} // namespace KarkinolutionView