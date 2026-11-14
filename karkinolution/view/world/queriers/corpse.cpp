#include "karkinolution/networking/client_side/interpreters/corpse.hpp"

#include "karkinolution/view/client/client.hpp"
#include "shared/core/id.hpp"

#include <karkinolution/view/world/queriers/corpse.hpp>

std::optional<Karkinolution::Binary::Interpreters::Models::Corpse::Corpse>
KarkinolutionView::World::Queriers::Corpse::see_corpse(KShared::Core::Id::BaseIdType      id,
													   KarkinolutionView::Client::Client &client) {
	return CorpseInterpreter::see_corpse(client, id);
}
