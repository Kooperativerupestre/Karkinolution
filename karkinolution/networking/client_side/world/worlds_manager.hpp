#pragma once

#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/networking/client_side/error.hpp"
#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/system.hpp"
#include "karkinolution/networking/client_side/world/world.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/core/error.hpp"
#include "shared/functional/data_structures.hpp"
#include "shared/functional/double_optional_output_enum.hpp"
#include "shared/functional/optional_reference.hpp"
#include "shared/functional/update_output_enum.hpp"

#include <mutex>
#include <optional>
#include <unordered_map>

namespace Karkinolution {
	namespace Networking {
		namespace ClientSide {
			namespace World {

				enum class WorldInexistenceReason : uint8_t {
					NO_EXISTS,
					NO_EMPLACED
				};

				struct MutableStoredWorld {
						KShared::Functional::OptionalReference<World> world;
						std::optional<WorldInexistenceReason>         inexistence_reason;
				};

				struct ImutableStoredWorld {
						KShared::Functional::OptionalReference<const World> world;
						std::optional<WorldInexistenceReason>               inexistence_reason;
				};

				class WorldStorage {
					private:

						std::unordered_map<WorldId, std::optional<World>> worlds;
						mutable std::mutex                                mutex;

						bool has_unlocked_(WorldId id) const {
							return worlds.contains(id);
						}

						bool remove_unlocked(WorldId id) {
							if (!has_unlocked_(id)) {
								return false;
							}
							worlds.erase(id);
							return true;
						}

						bool add_unlocked(WorldId id) {
							if (has_unlocked_(id)) {
								return false;
							}
							worlds[id] = std::nullopt;
							return true;
						}

						bool emplace_unlocked(const World &world) {
							if (!has_unlocked_(world.get_id())) {
								return false;
							}
							worlds[world.get_id()] = world;
							return true;
						}

						KShared::Functional::UpdateOutput update_unlocked(const World &world) {
							if (!has_unlocked_(world.get_id())) {
								return KShared::Functional::UpdateOutput::DOES_NOT_EXIST;
							}

							const auto has_emplaced = emplace_unlocked(world);

							assert(has_emplaced);
							return KShared::Functional::UpdateOutput::UPDATED;
						}

						std::vector<WorldId> see_all_world_ids_unlocked_() const {
							std::vector<WorldId> ids;
							ids.reserve(worlds.size());

							for (const auto &[id, _] : worlds) {
								ids.push_back(id);
							}
							return ids;
						}

					public:


						bool has(WorldId id) const {
							std::lock_guard lock{mutex};

							return worlds.contains(id);
						}

						KShared::Functional::DoubleOptional get_state(WorldId id) const {
							std::lock_guard lock{mutex};

							if (!has_unlocked_(id)) {
								return KShared::Functional::DoubleOptional::DOES_NOT_EXIST;
							}

							const auto &world = worlds.at(id);

							if (world.has_value()) {
								return KShared::Functional::DoubleOptional::EXISTS_AND_HAS;
							}
							return KShared::Functional::DoubleOptional::EXISTS_AND_HAS_NOT;
						}

						// Make the local set of IDs exactly match the server snapshot.
						void sync(const std::vector<WorldId> &all_world_ids) {
							std::lock_guard lock{mutex};
							for (size_t i = 0; i < all_world_ids.size(); i++) {
								const auto has_here = has_unlocked_(all_world_ids[i]);

								if (!has_here) {
									const auto has_added = add_unlocked(all_world_ids[i]);
									assert(has_added);
								}
							}

							const auto all_ids = see_all_world_ids_unlocked_();
							for (const auto id : all_ids) {
								bool has_there = false;

								for (size_t i = 0; i < all_world_ids.size(); i++) {
									if (all_world_ids[i] == id) {
										has_there = true;
									}
								}

								if (!has_there) {
									const auto has_removed = remove_unlocked(id);
									assert(has_removed);
								}
							}
						}

						// A missing reference means that the world ID is unknown; the reason
						// distinguishes that state from a known but unemplaced world.
						MutableStoredWorld get_world(WorldId id) {
							std::lock_guard lock{mutex};
							if (!has_unlocked_(id)) {
								return {.world = nullptr, .inexistence_reason = WorldInexistenceReason::NO_EXISTS};
							}

							auto &world = worlds.at(id);

							if (!world) {
								return {.world = nullptr, .inexistence_reason = WorldInexistenceReason::NO_EMPLACED};
							}

							return {.world = world.value(), .inexistence_reason = std::nullopt};
						}

						ImutableStoredWorld get_world(WorldId id) const {
							std::lock_guard lock{mutex};

							if (!has_unlocked_(id)) {
								return {.world = nullptr, .inexistence_reason = WorldInexistenceReason::NO_EXISTS};
							}

							const auto &world = worlds.at(id);

							if (!world) {
								return {.world = nullptr, .inexistence_reason = WorldInexistenceReason::NO_EMPLACED};
							}

							return {.world = world.value(), .inexistence_reason = std::nullopt};
						}

						bool emplace(const World &world) {
							std::lock_guard lock{mutex};
							if (!has_unlocked_(world.get_id())) {
								return false;
							}
							worlds[world.get_id()] = world;
							return true;
						}

						std::vector<WorldId> see_all_world_ids() const {
							std::lock_guard lock{mutex};
							return see_all_world_ids_unlocked_();
						}
				};


				enum class SetWorldOutput : uint8_t {
					NO_EXISTS,
					EXISTS_BUT_COULD_NOT,
					EXISTS_AND_COULD,
					ALREADY_IS_SET,
					SERVER_SIDE_ERROR // Used when the configuration request receives no server response.
				};

				enum class ClearCurrentWorldIdOutput : uint8_t {
					IS_NOT_SET,
					CLEANED
				};

				template <AsClient ClientT> class WorldsManager {
					private:

						ClientT &client;

						WorldStorage           world_storage;
						std::optional<WorldId> current_world_id = std::nullopt;


						mutable std::mutex mutex;

						void validate_is_connected_unlocked_() const {
							if (!client.connect()) {
								throw ClientError("The client of WorldsManager should be connected");
							}
						}

						void validate_is_connected_unlocked_(const std::string &message) const {
							if (!client.is_connected()) {
								throw ClientError(message);
							}
						}

						bool has_current_world_unlocked_() const {
							return current_world_id.has_value();
						}

						// This is also false when no current world ID has been selected.
						bool current_world_exists_unlocked_() const {
							if (!current_world_id.has_value()) {
								return false;
							}
							return world_storage.has(current_world_id.value());
						}

						bool world_inexistence_is_broken_unlocked_() const {
							// A selected world must remain known while this manager owns the
							// client connection; external deletion would violate that boundary.
							return current_world_id.has_value() && !world_storage.has(current_world_id.value());
						}

						bool world_emplacement_is_broken_unlocked_() const {
							// Selecting a world must also emplace its local representation.
							return current_world_id.has_value()
								&& world_storage.get_state(current_world_id.value())
								== KShared::Functional::DoubleOptional::EXISTS_AND_HAS_NOT;
						}

						void sync_world_ids_unlocked_() {
							validate_is_connected_unlocked_(
								"The client of WorldsManager should be connected to sync world ids");

							const auto world_ids = Interpreters::System::get_all_world_ids(client);

							world_storage.sync(world_ids);

							assert(!world_emplacement_is_broken_unlocked_());
							assert(!world_inexistence_is_broken_unlocked_());
						}

					public:


						WorldsManager(ClientT &client)
							: client(client) {


							if (client.is_connected()) {
								sync_world_ids_unlocked_();
							}
						}

						bool world_inexistence_is_broken() const {
							std::lock_guard lock{mutex};
							return world_inexistence_is_broken_unlocked_();
						}

						void sync_after_client_connects() {
							std::lock_guard lock{mutex};
							assert(client.is_connected());
							sync_world_ids_unlocked_();
						}

						bool world_emplacement_is_broken() const {
							std::lock_guard lock{mutex};
							return world_emplacement_is_broken_unlocked_();
						}

						bool has_world(WorldId id) const {
							std::lock_guard lock{mutex};
							return world_storage.has(id);
						}

						bool has_current_world() const {
							std::lock_guard lock{mutex};
							return has_current_world_unlocked_();
						}

						// This is also false when no current world ID has been selected.
						bool current_world_exists() const {
							std::lock_guard lock{mutex};
							return current_world_exists_unlocked_();
						}

						KShared::Functional::OptionalReference<const World> see_current_world() const {
							std::lock_guard lock{mutex};

							if (!current_world_id) {
								return nullptr;
							}

							if (!world_storage.has(current_world_id.value())) {
								return nullptr;
							}
							return world_storage.get_world(current_world_id.value()).world;
						}

						KShared::Functional::OptionalReference<World> see_current_world() {
							std::lock_guard lock{mutex};

							if (!current_world_id) {
								return nullptr;
							}
							if (!world_storage.has(current_world_id.value())) {
								return nullptr;
							}
							return world_storage.get_world(current_world_id.value()).world;
						}

						// Return a snapshot so callers cannot observe the storage while it is locked.
						std::vector<WorldId> see_all_world_ids() const {
							std::lock_guard lock{mutex};

							return world_storage.see_all_world_ids();
						}

						ClearCurrentWorldIdOutput clear_current_world() {
							std::lock_guard lock{mutex};

							if (!has_current_world_unlocked_()) {
								return ClearCurrentWorldIdOutput::IS_NOT_SET;
							}

							current_world_id = std::nullopt;
							return ClearCurrentWorldIdOutput::CLEANED;
						}

						// Any result other than EXISTS_AND_COULD leaves current_world_id unset.
						// Selecting another world is forbidden until the current one is cleared.
						SetWorldOutput set_current_world(WorldId id) {
							std::lock_guard lock{mutex};
							if (has_current_world_unlocked_()) {
								return SetWorldOutput::ALREADY_IS_SET;
							}

							if (!world_storage.has(id)) {
								return SetWorldOutput::NO_EXISTS;
							}

							const auto has_configured = Interpreters::System::config(client, id);

							if (!has_configured) {
								return SetWorldOutput::SERVER_SIDE_ERROR;
							}

							if (!has_configured.value()) {
								return SetWorldOutput::EXISTS_BUT_COULD_NOT;
							}

							current_world_id = id;
							world_storage.emplace(World{id});
							return SetWorldOutput::EXISTS_AND_COULD;
						}
				};

			} // namespace World
		} // namespace ClientSide
	} // namespace Networking
} // namespace Karkinolution
