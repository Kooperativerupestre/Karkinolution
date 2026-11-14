#pragma once

#include "karkinolution/networking/server_side/server/server.hpp"
#include "karkinolution/world/world.hpp"
#include "karkinolution/world/world_storage.hpp"

#include <asio/io_context.hpp>
#include <optional>

namespace Karkinolution::Networking::ServerSide::System {
	class System {
		private:

			asio::io_context io_context;

			std::optional<Server::Server> server;
			World::WorldStorage           world_storage;

		public:

			void stop_server();
			void create(const World::WorldPreset &preset);
	};
} // namespace Karkinolution::Networking::ServerSide::System
