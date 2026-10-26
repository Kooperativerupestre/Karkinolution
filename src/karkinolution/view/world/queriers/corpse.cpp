#include "karkinolution/networking/sdk/src/interpreters/corpse.hpp"

#include "karkinolution/core/id.hpp"
#include "karkinolution/view/client/client.hpp"

#include <karkinolution/view/world/queriers/corpse.hpp>

std::optional<Karkinolution::Binary::Interpreters::Models::Corpse::Corpse>
KarkinolutionView::World::Queriers::Corpse::see_corpse(BaseIdType                         id,
													   KarkinolutionView::Client::Client &client) {
	return KarkinolutionSDK::Interpreters::Corpse::see_corpse(client, id);
}
