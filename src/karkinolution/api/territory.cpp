#include <karkinolution/api/territory.hpp>

Karkinolution::Binary::Interpreters::Models::Territory::Territory
Karkinolution::API::Territory::see_territory(const Karkinolution::World::World &world) {
	return Karkinolution::Binary::Interpreters::Models::Territory::Territory{
		world.see_territory().size()};
}
