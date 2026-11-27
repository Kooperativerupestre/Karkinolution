#include "app/aliases.hpp"
#include "shared/functional/optional_reference.hpp"

#include <app/world_environment.hpp>

namespace KApp {
	void WorldEnvironment::connect() {
		client.connect();
		if (client.is_connected()) {
			worlds_manager.sync_after_client_connects();
		}
	}

	void WorldEnvironment::disconnect() {
		client.disconnect();
	}

	bool WorldEnvironment::is_connected() const {
		return client.is_connected();
	}

	bool WorldEnvironment::has_current_world() const {
		return worlds_manager.has_current_world();
	}

	KShared::Functional::OptionalReference<World> WorldEnvironment::get_current_world() {
		return worlds_manager.see_current_world();
	}

	KShared::Functional::OptionalReference<const World> WorldEnvironment::get_current_world() const {
		return worlds_manager.see_current_world();
	}

	WorldResponseResult WorldEnvironment::request_world_scope(const WorldRequestData &data) {
		return client.request_world_scope(data);
	}

	SystemResponseResult WorldEnvironment::request_system_scope(const SystemRequestData &data) {
		return client.request_system_scope(data);
	}


} // namespace KApp