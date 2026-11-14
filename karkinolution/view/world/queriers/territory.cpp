#include <karkinolution/networking/client_side/interpreters/territory.hpp>
#include <karkinolution/view/world/queriers/territory.hpp>

std::optional<Karkinolution::Binary::Interpreters::Models::Territory::Territory>
KarkinolutionView::World::Queriers::Territory::see_territory(KarkinolutionView::Client::Client &client) {
	return TerritoryInterpreter::see_territory(client);
}
