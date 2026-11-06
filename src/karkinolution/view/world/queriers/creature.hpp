#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/view/client/client.hpp"

#include <karkinolution/binary/interpreters/models/creature.hpp>

namespace KarkinolutionView {
	namespace World {
		namespace Queriers {
			namespace Creature {
				std::optional<Karkinolution::Binary::Interpreters::Models::Creature::Creature>
				see_creature(Karkinolution::Core::Id::BaseIdType id, KarkinolutionView::Client::Client &client);
			} // namespace Creature
		} // namespace Queriers
	} // namespace World
} // namespace KarkinolutionView