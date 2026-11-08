#pragma once
#include "karkinolution/networking/sdk/src/interpreters/concepts.hpp"
#include "karkinolution/networking/sdk/src/interpreters/creature.hpp"
#include "karkinolution/networking/sdk/src/interpreters/internal_aliases.hpp"
#include "karkinolution/networking/sdk/src/interpreters/soil.hpp"
#include "karkinolution/networking/sdk/src/interpreters/territory.hpp"
#include "shared/core/id.hpp"
#include "shared/functional/unreachable_or_throw.hpp"
#include "shared/math/units.hpp"

#include <karkinolution/networking/sdk/src/interpreters/corpse.hpp>
#include <karkinolution/networking/sdk/src/world/aliases.hpp>
#include <vector>

namespace KarkinolutionSDK {
	namespace World {
		enum class SyncOutput : uint8_t {
			DOES_NOT_EXIST,
			REMOVED,
			UPDATED
		};

		class Territory {
			private:

				std::vector<Soil> soils;

				std::optional<KShared::Math::Units::Size> size;


				bool remove_soil(SoilId id);
				bool update(const Soil &soil);

				template <HasRequest T> SyncOutput sync_soil(T &client, SoilId id) {
					if (!has_soil(id)) {
						return SyncOutput::DOES_NOT_EXIST;
					}

					const auto output = Interpreters::Soil::see_soil(client, id);

					if (!output) {
						assert(remove_soil(id));
						return SyncOutput::REMOVED;
					}

					assert(update(*output));
					return SyncOutput::UPDATED;
				}

				template <HasRequest T> void sync_size(T &client) {
					const auto output = Interpreters::Territory::see_territory(client);

					if (output) {
						size = output.value().size;
					}
				}

			public:

				Territory() {}

				bool add(const Soil &soil);
				bool has_soil(SoilId id) const;

				template <HasRequest T> void sync(T &client) {
					sync_size(client);
					std::vector<SoilId> soil_ids;

					soil_ids.reserve(soils.size());

					for (size_t i = 0; i < soil_ids.size(); i++) {
						sync_soil(client, soil_ids[i]);
					}
				}

				const std::vector<Soil> &get_soils() const {
					return soils;
				}

				const KShared::Math::Units::Size &get_size() const {
					if (size) {
						return *size;
					}
					KShared::Functional::unreachable_or_throw(
						"The get size of client world can't be called with size == std::nullopt");
				}
		};

		class EntitiesStorage {
			private:

				std::vector<Creature> creatures;
				std::vector<Corpse>   corpses;


				bool remove_creature(OrganismId id);
				bool remove_corpse(OrganismId id);
				bool remove(OrganismId id);

				bool update(const Creature &new_creature);
				bool update(const Corpse &new_corpse);

				template <HasRequest T> SyncOutput sync_creature(OrganismId id, T &client) {
					if (!has_creature(id)) {
						return SyncOutput::DOES_NOT_EXIST;
					}
					const auto output = Interpreters::Creature::see_creature(client, id);

					if (!output) {
						assert(remove_creature(id));
						return SyncOutput::REMOVED;
					}

					assert(update(*output));
					return SyncOutput::UPDATED;
				}

				template <HasRequest T> SyncOutput sync_corpse(OrganismId id, T &client) {
					if (!has_corpse(id)) {
						return SyncOutput::DOES_NOT_EXIST;
					}

					const auto output = Interpreters::Corpse::see_corpse(client, id);

					if (!output) {
						assert(remove_corpse(id));
						return SyncOutput::REMOVED;
					}
					assert(update(output));
					return SyncOutput::UPDATED;
				}


			public:

				EntitiesStorage() {}

				bool has_creature(OrganismId id) const;
				bool has_corpse(OrganismId id) const;

				bool add(const Creature &creature);
				bool add(const Corpse &corpse);

				template <HasRequest T> void sync(T &client) {
					std::vector<OrganismId> creature_ids;
					std::vector<OrganismId> corpse_ids;

					creature_ids.reserve(creatures.size());
					corpse_ids.reserve(corpses.size());

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
		};

		class World {
			private:

				EntitiesStorage entities;
				Territory       territory;

			public:

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

				bool has_creature(OrganismId id) const;
				bool has_corpse(OrganismId id) const;
				bool has_soil(SoilId id) const;

				template <HasRequest T> void sync(T &client) {
					entities.sync(client);
					territory.sync(client);
				}
		};
	} // namespace World
} // namespace KarkinolutionSDK
