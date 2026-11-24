#include "app/creature_creator.hpp"

#include "karkinolution/networking/client_side/interpreters/creature.hpp"
#include "karkinolution/networking/client_side/interpreters/system.hpp"
#include "karkinolution/networking/client_side/interpreters/territory.hpp"

#include <chrono>
#include <future>
#include <iostream>

namespace KarkinolutionView {
	using Client                   = Karkinolution::Networking::ClientSide::Client::RuntimeClient;
	namespace CreatureInterpreter  = Karkinolution::Networking::ClientSide::Interpreters::Creature;
	namespace SystemInterpreter    = Karkinolution::Networking::ClientSide::Interpreters::System;
	namespace TerritoryInterpreter = Karkinolution::Networking::ClientSide::Interpreters::Territory;

	bool CreatureCreator::busy() const {
		return pending_.valid();
	}

	void CreatureCreator::acquire_world() {
		if (!client_.is_connected() || pending_.valid()) {
			return;
		}

		pending_ = std::async(std::launch::async, [this] {
			OperationResult result;
			try {
				const auto world_ids = SystemInterpreter::get_all_world_ids(client_);
				if (world_ids.empty()) {
					result.error = "The server has no worlds available";
					return result;
				}

				const auto configured = SystemInterpreter::config(client_, world_ids.front());
				if (!configured || !configured.value()) {
					result.error = "The server refused world acquisition";
					return result;
				}

				result.world_acquired = true;
				result.territory      = TerritoryInterpreter::see_territory(client_);
			} catch (...) {
				try {
					throw;
				} catch (const std::exception &error) {
					result.error = error.what();
				} catch (...) {
					result.error = "Unknown error while acquiring a world";
				}
			}
			return result;
		});
	}

	void CreatureCreator::create() {
		if (!world_ready_) {
			std::cerr << "Cannot create creature: no world is connected\n";
			return;
		}
		if (pending_.valid()) {
			return;
		}

		pending_ = std::async(std::launch::async, [this] {
			OperationResult result;
			result.creation_attempted = true;
			try {
				result.creature =
					CreatureInterpreter::make_creature(client_, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
			} catch (...) {
				try {
					throw;
				} catch (const std::exception &error) {
					result.error = error.what();
				} catch (...) {
					result.error = "Unknown error while creating a creature";
				}
			}
			return result;
		});
	}

	CreatureCreator::PollResult CreatureCreator::poll() {
		if (!pending_.valid() || pending_.wait_for(std::chrono::milliseconds{0}) != std::future_status::ready) {
			return PollResult::NONE;
		}

		const OperationResult result = pending_.get();
		if (!result.error.empty()) {
			std::cerr << result.error << '\n';
			world_ready_ = false;
			return PollResult::FAILED;
		}
		if (result.world_acquired) {
			world_ready_ = true;
			if (result.territory) {
				territory_size_ = result.territory->size;
			}
			return PollResult::WORLD_READY;
		}
		if (result.territory) {
			territory_size_ = result.territory->size;
		}
		if (result.creature) {
			creatures_.push_back(*result.creature);
			std::cout << "Creature created: " << result.creature->id << '\n';
			return PollResult::CREATURE_CREATED;
		}
		if (result.creation_attempted) {
			std::cerr << "The server returned no creature\n";
			return PollResult::CREATION_FAILED;
		}
		return PollResult::FAILED;
	}
} // namespace KarkinolutionView
