#include "karkinolution/networking/server.hpp"
#include "karkinolution/world/world.hpp"

#include <asio/io_context.hpp>
#include <karkinolution/networking/port.hpp>

static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);
static_assert(sizeof(int) == 4);
static_assert(sizeof(long long) == 8);

int main() {
	asio::io_context io_context;

	Karkinolution::World::WorldPreset preset{
		.seed    = 1,
		.size    = KShared::Math::Units::Size{.width  = KShared::Math::Units::Width{100.0},
											  .height = KShared::Math::Units::Height{100.0}},
		.epsilon = 5.0f};

	Karkinolution::World::World       world{preset};
	Karkinolution::Networking::Server server(io_context, Karkinolution::Networking::PORT, world);

	io_context.run();

	return 0;
}
