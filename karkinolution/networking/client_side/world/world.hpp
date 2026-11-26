#pragma once

#include "karkinolution/networking/client_side/interpreters/concepts.hpp"
#include "karkinolution/networking/client_side/interpreters/corpse.hpp"
#include "karkinolution/networking/client_side/interpreters/creature.hpp"
#include "karkinolution/networking/client_side/interpreters/internal_aliases.hpp"
#include "karkinolution/networking/client_side/interpreters/soil.hpp"
#include "karkinolution/networking/client_side/interpreters/territory.hpp"
#include "karkinolution/networking/client_side/world/aliases.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/core/id.hpp"
#include "shared/functional/optional_reference.hpp"
#include "shared/functional/unreachable_or_throw.hpp"
#include "shared/math/units.hpp"

#include <cassert>
#include <optional>
#include <vector>

namespace Karkinolution::Networking::ClientSide::World {
	enum class SyncOutput : uint8_t {
		DOES_NOT_EXIST,
		REMOVED,
		UPDATED
	};

	using WorldId = Karkinolution::World::WorldId;

	class Territory {
		private:

			std::vector<Soil> soils;

			std::optional<KShared::Math::Units::Size> size;
			bool                                      has_loaded_;

			template <CanRequestWorldScope T> SyncOutput sync_soil(T &client, SoilId id) {
				if (!has_soil(id)) {
					return SyncOutput::DOES_NOT_EXIST;
				}

				const auto output = Interpreters::Soil::see_soil(client, id);

				if (!output) {
					const bool removed = remove_soil(id);
					assert(removed);
					if (!removed) {
						return SyncOutput::DOES_NOT_EXIST;
					}
					return SyncOutput::REMOVED;
				}

				const bool updated = update(*output);
				assert(updated);
				if (!updated) {
					return SyncOutput::DOES_NOT_EXIST;
				}
				return SyncOutput::UPDATED;
			}

			template <CanRequestWorldScope T> void sync_size(T &client) {
				const auto output = Interpreters::Territory::see_territory(client);

				if (output) {
					size = output.value().size;
				}
			}

		public:

			Territory() {}

			bool add(const Soil &soil);
			bool update(const Soil &soil);
			bool remove_soil(SoilId id);
			bool has_soil(SoilId id) const;

			const std::vector<Soil> &get_soils() const {
				return soils;
			}

			KShared::Functional::OptionalReference<const Soil> get_a_soil(SoilId id) const;

			const KShared::Math::Units::Size &get_size() const {
				if (!size) {
					throw std::runtime_error("Territory size is not initialized");
				}
				return size.value();
			}

			bool has_loaded() const {
				return has_loaded_;
			}

			bool can_load() const {
				return !has_loaded_ && soils.empty() && !size.has_value();
			}

			template <CanRequestWorldScope T> void sync(T &client) {
				sync_size(client);

				std::vector<SoilId> ids;
				ids.reserve(soils.size());

				for (size_t i = 0; i < soils.size(); i++) {
					ids.push_back(soils[i].id);
				}

				for (size_t i = 0; i < ids.size(); i++) {
					sync_soil(client, ids[i]);
				}
			}

			template <CanRequestWorldScope T> bool load(T &client) {
				if (!can_load()) {
					return false;
				}

				const auto all_soil_ids = Interpreters::Soil::see_all_soils(client);

				for (const auto id : all_soil_ids) {
					const auto soil = Interpreters::Soil::see_soil(client, id);

					assert(soil);

					const auto has_added = add(soil.value());

					assert(has_added);
				}

				const auto optional_size = Interpreters::Territory::see_territory(client);
				assert(optional_size);
				size        = optional_size.value();
				has_loaded_ = true;
				return true;
			}
	};

	class EntitiesStorage {
		private:

			std::vector<Creature> creatures;
			std::vector<Corpse>   corpses;

			bool has_loaded_ = false;

			template <CanRequestWorldScope T> SyncOutput sync_creature(T &client, OrganismId id) {
				if (!has_creature(id)) {
					return SyncOutput::DOES_NOT_EXIST;
				}

				const auto output = Interpreters::Creature::see_creature(client, id);

				if (!output) {
					const bool removed = remove_creature(id);
					assert(removed);
					if (!removed) {
						return SyncOutput::DOES_NOT_EXIST;
					}
					return SyncOutput::REMOVED;
				}

				const bool updated = update(*output);
				assert(updated);
				if (!updated) {
					return SyncOutput::DOES_NOT_EXIST;
				}
				return SyncOutput::UPDATED;
			}

			template <CanRequestWorldScope T> SyncOutput sync_corpse(T &client, OrganismId id) {
				if (!has_corpse(id)) {
					return SyncOutput::DOES_NOT_EXIST;
				}

				const auto output = Interpreters::Corpse::see_corpse(client, id);

				if (!output) {
					const bool removed = remove_corpse(id);
					assert(removed);
					if (!removed) {
						return SyncOutput::DOES_NOT_EXIST;
					}
					return SyncOutput::REMOVED;
				}

				const bool updated = update(*output);
				assert(updated);
				if (!updated) {
					return SyncOutput::DOES_NOT_EXIST;
				}
				return SyncOutput::UPDATED;
			}

		public:

			EntitiesStorage() {}

			bool add(const Creature &creature);
			bool add(const Corpse &corpse);
			bool update(const Creature &creature);
			bool update(const Corpse &corpse);
			bool update(const Soil &soil);
			bool remove_creature(OrganismId id);
			bool remove_corpse(OrganismId id);
			bool remove_soil(SoilId id);

			bool has_creature(OrganismId id) const;
			bool has_corpse(OrganismId id) const;

			KShared::Functional::OptionalReference<const Creature> get_a_creature(OrganismId id) const;
			KShared::Functional::OptionalReference<const Corpse>   get_a_corpse(OrganismId id) const;

			template <CanRequestWorldScope T> void sync(T &client) {
				std::vector<OrganismId> creature_ids;
				std::vector<OrganismId> corpse_ids;

				creature_ids.reserve(creatures.size());
				corpse_ids.reserve(corpses.size());

				for (size_t i = 0; i < creatures.size(); i++) {
					creature_ids.push_back(creatures[i].id);
				}

				for (size_t i = 0; i < corpses.size(); i++) {
					corpse_ids.push_back(corpses[i].id);
				}

				for (size_t i = 0; i < creature_ids.size(); i++) {
					sync_creature(client, creature_ids[i]);
				}
				for (size_t i = 0; i < corpse_ids.size(); i++) {
					sync_corpse(client, corpse_ids[i]);
				}
			}

			bool has(OrganismId id) const;

			const std::vector<Creature> &get_creatures() const {
				return creatures;
			}

			const std::vector<Corpse> &get_corpses() const {
				return corpses;
			}

			bool has_loaded() const {
				return has_loaded_;
			}

			bool can_load() const {
				return !has_loaded_ && corpses.empty() && creatures.empty();
			}

			template <CanRequestWorldScope T> bool load(T &client) {
				if (!can_load()) {
					return false;
				}
				const auto all_corpse_ids   = Interpreters::Corpse::see_all_corpses(client);
				const auto all_creature_ids = Interpreters::Creature::see_all_creatures(client);
				for (size_t i = 0; i < all_corpse_ids.size(); i++) {
					const auto corpse = Interpreters::Corpse::see_corpse(client, all_corpse_ids[i]);

					assert(corpse);
					const auto has_added = add(corpse);
					assert(has_added);
				}
				for (size_t i = 0; i << all_creature_ids.size(); i++) {
					const auto creature = Interpreters::Creature::see_creature(client, all_creature_ids[i]);

					assert(creature);
					const auto has_added = add(creature);
					assert(has_added);
				}
				has_loaded_ = true;
				return true;
			}
	};

	class World {
		private:

			EntitiesStorage entities;
			Territory       territory;

			WorldId id;

			bool has_loaded_ = false;

			bool can_load() const {
				return entities.can_load() && territory.can_load();
			}

		public:


			World(WorldId id)
				: id(id) {}

			// load is an idempotent operation intended to synchronize the local world with the world stored on the
			// server, performing the initial synchronization only once.
			//
			// Currently, this implementation assumes that, once a server-side World has been selected here, it will not
			// be modified by any other client. This assumption is obviously false in the current architecture, where
			// multiple clients may interact with the same World.
			//
			// I deliberately started with this simplified model before implementing more advanced synchronization
			// mechanisms.
			template <CanRequestWorldScope T> bool load(T &client) {
				if (!can_load()) {
					return false;
				}

				const auto entities_has_loaded  = entities.load(client);
				const auto territory_has_loaded = territory.load(client);
				assert(entities_has_loaded);
				assert(territory_has_loaded);
				has_loaded_ = true;
				return true;
			}

			WorldId get_id() const {
				return id;
			}

			const std::vector<Creature> &get_creatures() const {
				return entities.get_creatures();
			}

			const std::vector<Corpse> &get_corpses() const {
				return entities.get_corpses();
			}

			const std::vector<Soil> &get_soils() const {
				return territory.get_soils();
			}

			const KShared::Math::Units::Size &get_size() const {
				return territory.get_size();
			}

			bool add(const Soil &soil);
			bool add(const Creature &creature);
			bool add(const Corpse &corpse);
			bool update(const Creature &creature);
			bool update(const Corpse &corpse);
			bool update(const Soil &soil);

			KShared::Functional::OptionalReference<const Creature> get_a_creature(OrganismId id) const;
			KShared::Functional::OptionalReference<const Corpse>   get_a_corpse(OrganismId id) const;
			KShared::Functional::OptionalReference<const Soil>     get_a_soil(SoilId id) const;

			bool has_creature(OrganismId id) const;
			bool has_corpse(OrganismId id) const;
			bool has_soil(SoilId id) const;
			bool remove_creature(OrganismId id);
			bool remove_corpse(OrganismId id);
			bool remove_soil(SoilId id);

			template <CanRequestWorldScope T> void sync(T &client) {
				entities.sync(client);
				territory.sync(client);
			}
	};
} // namespace Karkinolution::Networking::ClientSide::World
