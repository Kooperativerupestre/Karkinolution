#include "karkinolution/networking/server_side/system/system.hpp"

#include "karkinolution/networking/port.hpp"
#include "karkinolution/world/world.hpp"

#include <cassert>

namespace Karkinolution::Networking::ServerSide::System {
	void System::stop_server() {
		if (server) {
			server->stop();
		}
	}

	void System::create(const World::WorldPreset &preset) {
		World::World world{preset};

		assert(world_storage.add(std::move(world)));
		if (!server) {
			server.emplace(io_context, PORT, world_storage);
		}
	}
} // namespace Karkinolution::Networking::ServerSide::System
