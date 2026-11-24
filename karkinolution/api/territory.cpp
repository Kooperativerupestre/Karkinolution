#include <karkinolution/api/territory.hpp>

Karkinolution::Networking::Frames::World::Interpreters::Models::Territory::Territory
Karkinolution::API::Territory::see_territory(const Karkinolution::World::World &world) {
	return Karkinolution::Networking::Frames::World::Interpreters::Models::Territory::Territory{
		world.see_territory().size()};
}
