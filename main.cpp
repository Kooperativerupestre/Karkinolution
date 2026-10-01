#include "karkinolution/networking/server.hpp"
#include "karkinolution/world/world.hpp"

#include <asio/io_context.hpp>
#include <karkinolution/networking/port.hpp>

int main() {
	asio::io_context io_context;

	WorldPreset preset{
		.seed    = 1,
		.size    = Size{.lateral = Lateral{100.0}, .height = Height{100.0}, .depth = Depth{100.0}},
		.epsilon = 5.0f};

	auto   world = WorldFactory::create_world(preset);
	Server server(io_context, PORT, world);

	io_context.run();

	return 0;
}