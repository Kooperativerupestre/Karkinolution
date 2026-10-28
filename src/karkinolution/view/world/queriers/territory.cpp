#include <karkinolution/networking/sdk/src/interpreters/territory.hpp>
#include <karkinolution/view/world/queriers/territory.hpp>

std::optional<Karkinolution::Binary::Interpreters::Models::Territory::Territory>
KarkinolutionView::World::Queriers::Territory::see_territory(
	KarkinolutionView::Client::Client &client) {
	return KarkinolutionSDK::Interpreters::Territory::see_territory(client);
}
