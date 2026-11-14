#include "karkinolution/binary/interpreters/models/soil.hpp"

#include <karkinolution/networking/client_side/interpreters/soil.hpp>
#include <karkinolution/view/world/queriers/soil.hpp>

std::optional<Karkinolution::Binary::Interpreters::Models::Soil::Soil>
KarkinolutionView::World::Queriers::Soil::see_soil(KShared::Core::Id::BaseIdType      id,
												   KarkinolutionView::Client::Client &client) {
	return SoilInterpreter::see_soil(client, id);
}

std::vector<Karkinolution::Binary::Interpreters::Models::Soil::Soil>
KarkinolutionView::World::Queriers::Soil::see_all_soils(KarkinolutionView::Client::Client &client) {
	return SoilInterpreter::see_all_soils(client);
}