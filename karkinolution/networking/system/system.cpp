#include "karkinolution/world/world.hpp"

#include <karkinolution/networking/system/system.hpp>

namespace Karkinolution::Networking {
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
} // namespace Karkinolution::Networking
