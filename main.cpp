#include "karkinolution/networking/server.hpp"
#include "karkinolution/world/world.hpp"

#include <asio/io_context.hpp>
#include <karkinolution/networking/port.hpp>

int main() {
	asio::io_context io_context;

	Karkinolution::World::WorldPreset preset{
		.seed    = 1,
		.size    = Karkinolution::Math::Units::Size{.width  = Karkinolution::Math::Units::Width{100.0},
													.height = Karkinolution::Math::Units::Height{100.0}},
		.epsilon = 5.0f};

	Karkinolution::World::World       world{preset};
	Karkinolution::Networking::Server server(io_context, Karkinolution::Networking::PORT, world);

	io_context.run();

	return 0;
}
