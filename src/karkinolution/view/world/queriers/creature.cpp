#include <karkinolution/networking/sdk/src/interpreters/creature.hpp>
#include <karkinolution/view/world/queriers/creature.hpp>

std::optional<Karkinolution::Binary::Interpreters::Models::Creature::Creature>
KarkinolutionView::World::Queriers::Creature::see_creature(
	BaseIdType                         id,
	KarkinolutionView::Client::Client &client) {
	return KarkinolutionSDK::Interpreters::Creature::see_creature(client, id);
}
