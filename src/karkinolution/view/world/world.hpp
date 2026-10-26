#pragma once
#include "karkinolution/binary/interpreters/models/soil.hpp"
#include "karkinolution/core/id.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/view/client/client.hpp"

#include <karkinolution/binary/interpreters/models/corpse.hpp>
#include <karkinolution/binary/interpreters/models/creature.hpp>

namespace KarkinolutionView {
	namespace World {
		inline constexpr bool SYNC_TERRAIN_SIZE = false;


		enum class EntitySyncOutput : uint8_t {
			UPDATED       = 1,
			WAS_NOT_FOUND = 2,
			DOES_NOT_EXIST
		};

		class Terrain {
			private:

				Size                                                                 size;
				std::vector<Karkinolution::Binary::Interpreters::Models::Soil::Soil> soils;

				bool has_soil(BaseIdType id) const;
				bool remove_soil(BaseIdType id);
				bool add(const Karkinolution::Binary::Interpreters::Models::Soil::Soil &soil);
				bool
				update(const Karkinolution::Binary::Interpreters::Models::Soil::Soil &new_soil);

				void             sync_size(KarkinolutionView::Client::Client &client);
				EntitySyncOutput sync_soil(BaseIdType                         id,
										   KarkinolutionView::Client::Client &client);


			public:

				Terrain()
					: size(1000, 1000, 100) {}

				const std::vector<Karkinolution::Binary::Interpreters::Models::Soil::Soil> &
				get_soils() const {
					return soils;
				}

				void sync(KarkinolutionView::Client::Client &client);
		};

		class World {
			private:

				Terrain terrain;

				std::vector<Karkinolution::Binary::Interpreters::Models::Creature::Creature>
																						 creatures;
				std::vector<Karkinolution::Binary::Interpreters::Models::Corpse::Corpse> corpses;


				EntitySyncOutput sync_creature(BaseIdType                         id,
											   KarkinolutionView::Client::Client &client);
				EntitySyncOutput sync_corpse(BaseIdType                         id,
											 KarkinolutionView::Client::Client &client);


				bool add(const Karkinolution::Binary::Interpreters::Models::Creature::Creature
							 &creature);
				bool add(const Karkinolution::Binary::Interpreters::Models::Corpse::Corpse &corpse);

				bool remove_creature(BaseIdType id);
				bool remove_corpse(BaseIdType id);

				bool remove(BaseIdType id);

				bool update(const Karkinolution::Binary::Interpreters::Models::Creature::Creature
								&new_creature);
				bool update(
					const Karkinolution::Binary::Interpreters::Models::Corpse::Corpse &new_corpse);

				bool has_creature(BaseIdType id) const;
				bool has_corpse(BaseIdType id) const;
				bool has(BaseIdType id) const;

			public:

				const std::vector<Karkinolution::Binary::Interpreters::Models::Creature::Creature> &
				get_creatures() const {
					return creatures;
				}

				const std::vector<Karkinolution::Binary::Interpreters::Models::Corpse::Corpse> &
				get_corpses() const {
					return corpses;
				}

				const std::vector<Karkinolution::Binary::Interpreters::Models::Soil::Soil> &
				get_soils() const {
					return terrain.get_soils();
				}

				void sync(KarkinolutionView::Client::Client &client);
		};
	} // namespace World
} // namespace KarkinolutionView