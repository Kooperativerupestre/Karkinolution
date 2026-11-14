#include "karkinolution/global_aliasses.hpp"
#include "karkinolution/world/world.hpp"
#include "shared/core/id.hpp"
#include "shared/functional/optional_reference.hpp"

#include <karkinolution/world/world_storage.hpp>
#include <mutex>

namespace Karkinolution::World {
	std::vector<WorldId> WorldStorage::get_all_world_ids() const {
		std::lock_guard      lock{mutex};
		std::vector<WorldId> ids;

		ids.reserve(entries.size());
		for (const auto &entry : entries) {
			ids.push_back(entry->world_id());
		}
		return ids;
	}

	std::optional<size_t> WorldStorage::get_entries_index_unlocked(BaseIdType id) const {
		for (size_t i = 0; i < entries.size(); i++) {
			if (entries[i]->world_id() == id) {
				return i;
			}
		}

		return std::nullopt;
	}

	bool WorldStorage::has_unlocked(BaseIdType id) const {
		for (size_t i = 0; i < entries.size(); i++) {
			if (entries[i]->world_id() == id) {
				return true;
			}
		}
		return false;
	}

	bool WorldStorage::add(World &&world) {
		std::lock_guard lock{mutex};

		if (has_unlocked(world.get_id())) {
			return false;
		}
		entries.emplace_back(std::make_unique<Entry>(std::move(world)));
		return true;
	}

	OptionalReference<Entry> WorldStorage::get(BaseIdType id) {
		std::lock_guard lock{mutex};
		if (!has_unlocked(id)) {
			return {nullptr};
		}
		return *entries[get_entries_index_unlocked(id).value()];
	}

	OptionalReference<const Entry> WorldStorage::get(BaseIdType id) const {
		std::lock_guard lock{mutex};

		if (!has_unlocked(id)) {
			return nullptr;
		}
		return *entries[get_entries_index_unlocked(id).value()];
	}

	bool WorldStorage::remove(BaseIdType id) {
		std::lock_guard lock{mutex};


		const auto index = get_entries_index_unlocked(id);
		if (!index) {
			return false;
		}

		if (entries[index.value()]->gift().was_taken()) {
			return false;
		}
		entries.erase(entries.begin() + index.value());
		return true;
	}

	std::optional<GiftLease<World>> WorldStorage::take(BaseIdType id) {
		std::lock_guard lock{mutex};
		const auto      index = get_entries_index_unlocked(id);

		if (!index) {
			return std::nullopt;
		}

		auto &gift = entries[index.value()]->gift();
		if (gift.was_taken()) {
			return std::nullopt;
		}
		return gift.take();
	}

	EntryState WorldStorage::get_state(BaseIdType id) const {
		std::lock_guard lock{mutex};

		const auto index = get_entries_index_unlocked(id);

		if (!index) {
			return EntryState::NO_EXISTS;
		}

		const auto &entry     = entries.at(index.value());
		const auto  was_taken = entry->gift().was_taken();
		if (was_taken) {
			return EntryState::TAKEN;
		}
		return EntryState::NOT_TAKEN;
	}

	bool WorldStorage::has(BaseIdType id) const {
		std::lock_guard lock{mutex};
		for (size_t i = 0; i < entries.size(); i++) {
			if (entries[i]->world_id() == id) {
				return true;
			}
		}
		return false;
	}
} // namespace Karkinolution::World
