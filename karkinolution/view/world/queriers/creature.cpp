#include <karkinolution/networking/client_side/interpreters/creature.hpp>
#include <karkinolution/view/world/queriers/creature.hpp>

std::optional<Karkinolution::Binary::Interpreters::Models::Creature::Creature>
KarkinolutionView::World::Queriers::Creature::see_creature(KShared::Core::Id::BaseIdType      id,
														   KarkinolutionView::Client::Client &client) {
	return CreatureInterpreter::see_creature(client, id);
}
