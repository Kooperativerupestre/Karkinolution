#pragma once

#include "karkinolution/networking/client_side/client/client.hpp"
#include "karkinolution/networking/frame/world/interpreters/models/creature.hpp"
#include "karkinolution/networking/frame/world/interpreters/models/territory.hpp"

#include <future>
#include <optional>
#include <string>
#include <vector>

namespace KarkinolutionView {
	class CreatureCreator {
		private:

			using Client    = Karkinolution::Networking::ClientSide::Client::RuntimeClient;
			using Creature  = Karkinolution::Networking::Frames::World::Interpreters::Models::Creature::Creature;
			using Territory = Karkinolution::Networking::Frames::World::Interpreters::Models::Territory::Territory;

			struct OperationResult {
					std::optional<Creature>  creature;
					std::optional<Territory> territory;
					bool                     world_acquired     = false;
					bool                     creation_attempted = false;
					std::string              error;
			};

			Client                                   &client_;
			std::future<OperationResult>              pending_;
			std::optional<KShared::Math::Units::Size> territory_size_;
			std::vector<Creature>                     creatures_;
			bool                                      world_ready_ = false;

		public:

			enum class PollResult {
				NONE,
				WORLD_READY,
				CREATURE_CREATED,
				CREATION_FAILED,
				FAILED
			};

			explicit CreatureCreator(Client &client)
				: client_(client) {}

			void       acquire_world();
			void       create();
			PollResult poll();

			[[nodiscard]] bool busy() const;

			[[nodiscard]] bool world_ready() const noexcept {
				return world_ready_;
			}

			[[nodiscard]] const std::optional<KShared::Math::Units::Size> &territory_size() const noexcept {
				return territory_size_;
			}

			[[nodiscard]] const std::vector<Creature> &creatures() const noexcept {
				return creatures_;
			}
	};
} // namespace KarkinolutionView
