#pragma once

#include <app/aliases.hpp>
#include <app/commanders/concepts.hpp>

namespace KApp {
	class WorldEnvironment {
		private:

			KApp::Client client;

			WorldsManager worlds_manager;

		public:

			WorldEnvironment()
				: client()
				, worlds_manager(client) {}

			void connect();

			void disconnect();

			bool is_connected() const;

			bool has_current_world() const;

			KShared::Functional::OptionalReference<World> get_current_world();

			KShared::Functional::OptionalReference<const World> get_current_world() const;

			WorldResponseResult request_world_scope(const WorldRequestData &data);

			SystemResponseResult request_system_scope(const SystemRequestData &data);

			void throw_if_no_connected(const std::string &message) const {
				if (!client.is_connected()) {
					std::cerr << message << "\n";
					assert(client.is_connected());
				}
			}
	};

	static_assert(Commanders::CanRequestWorldScope<WorldEnvironment>);
	static_assert(Commanders::AsClient<WorldEnvironment>);

} // namespace KApp