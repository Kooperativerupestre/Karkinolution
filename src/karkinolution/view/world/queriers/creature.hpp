#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/view/client/client.hpp"

#include <karkinolution/binary/interpreters/creature.hpp>

namespace KarkinolutionView {
	namespace World {
		namespace Queriers {
			namespace Creature {
				std::optional<CreatureDSI::Models::Creature>
				see_creature(BaseIdType id, KarkinolutionView::Client::Client &client);
			} // namespace Creature
		} // namespace Queriers
	} // namespace World
} // namespace KarkinolutionView