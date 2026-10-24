#pragma once
#include "karkinolution/core/id.hpp"
#include "karkinolution/networking/sdk/src/client/alias.hpp"
#include "karkinolution/networking/sdk/src/client/client.hpp"
#include "karkinolution/view/client/client.hpp"

#include <karkinolution/binary/interpreters/corpse.hpp>
#include <karkinolution/binary/interpreters/creature.hpp>

namespace KarkinolutionView {
	namespace World {
		enum class EntitySyncOutput : uint8_t {
			UPDATED       = 1,
			WAS_NOT_FOUND = 2,
			DOES_NOT_EXIST
		};

		struct WorldPatch {

				std::vector<CreatureDSI::Models::Creature> creatures_to_add;
				std::vector<CorpseDSI::Models::Corpse>     corpses_to_add;

				std::vector<BaseIdType> creatures_to_remove;
				std::vector<BaseIdType> corpses_to_remove;

				WorldPatch &operator+=(const WorldPatch &other) {
					creatures_to_add.insert(creatures_to_add.end(),
											other.creatures_to_add.begin(),
											other.creatures_to_add.end());

					corpses_to_add.insert(corpses_to_add.end(),
										  other.corpses_to_add.begin(),
										  other.corpses_to_add.end());

					creatures_to_remove.insert(creatures_to_remove.end(),
											   other.creatures_to_remove.begin(),
											   other.creatures_to_remove.end());

					corpses_to_remove.insert(corpses_to_remove.end(),
											 other.corpses_to_remove.begin(),
											 other.corpses_to_remove.end());

					return *this;
				}

				WorldPatch operator+(const WorldPatch &other) const {
					WorldPatch result = *this;
					result += other;
					return result;
				}
		};

		class World {
			private:

				std::vector<CreatureDSI::Models::Creature> creatures;
				std::vector<CorpseDSI::Models::Corpse>     corpses;

				EntitySyncOutput sync_creature(BaseIdType                         id,
											   KarkinolutionView::Client::Client &client);
				EntitySyncOutput sync_corpse(BaseIdType                         id,
											 KarkinolutionView::Client::Client &client);

				bool add(const CreatureDSI::Models::Creature &creature);
				bool add(const CorpseDSI::Models::Corpse &corpse);

				bool remove_creature(BaseIdType id);
				bool remove_corpse(BaseIdType id);

				bool remove(BaseIdType id);

				bool update(const CreatureDSI::Models::Creature &new_creature);
				bool update(const CorpseDSI::Models::Corpse &new_corpse);

				bool has_creature(BaseIdType id) const;
				bool has_corpse(BaseIdType id) const;
				bool has(BaseIdType id) const;

			public:

				const std::vector<CreatureDSI::Models::Creature> &get_creatures() {
					return creatures;
				}

				const std::vector<CorpseDSI::Models::Corpse> &get_corpses() {
					return corpses;
				}

				void sync(KarkinolutionView::Client::Client &client);
				void apply(const WorldPatch &patch);
		};
	} // namespace World
} // namespace KarkinolutionView