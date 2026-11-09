#pragma once

#include "karkinolution/world/world.hpp"
#include "shared/core/error.hpp"
#include "shared/core/id.hpp"
#include "shared/functional/optional_reference.hpp"
#include "shared/functional/unique_gift.hpp"
#include "shared/functional/unreachable_or_throw.hpp"

#include <deque>
#include <karkinolution/global_aliasses.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

namespace Karkinolution {
	namespace World {


		class Entry {
			private:

				BaseIdType         world_id_;
				mutable std::mutex mutex;
				UniqueGift<World>  gift_;

			public:

				BaseIdType world_id() const {
					return world_id_;
				}

				UniqueGift<World> &gift() {
					std::lock_guard lock{mutex};
					return gift_;
				}

				const UniqueGift<World> &gift() const {
					std::lock_guard lock{mutex};
					return gift_;
				}

				explicit Entry(World &&world)
					: gift_(std::move(world))
					, world_id_(world.get_id()) {}
		};

		enum class EntryState : uint8_t {
			TAKEN,
			NOT_TAKEN,
			NO_EXISTS
		};

		class WorldStorage {
			private:

				mutable std::mutex                         mutex;
				mutable std::deque<std::unique_ptr<Entry>> entries;

				OptionalReference<Entry>       get(BaseIdType id);
				OptionalReference<const Entry> get(BaseIdType id) const;

				std::optional<size_t> get_entries_index_unlocked(BaseIdType id) const;

				bool has_unlocked(BaseIdType id) const;

			public:


				bool has(BaseIdType id) const;
				bool add(World &&world);
				bool remove(BaseIdType id);

				std::optional<GiftLease<World>> take(BaseIdType id);
				EntryState                      get_state(BaseIdType id) const;
		};
	} // namespace World
} // namespace Karkinolution