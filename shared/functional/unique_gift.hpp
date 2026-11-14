#pragma once
#include "shared/core/error.hpp"
#include "shared/functional/optional_reference.hpp"

#include <atomic>
#include <cassert>
#include <mutex>
#include <shared/functional/terminate.hpp>

namespace KShared {
	namespace Functional {
		class BadGiftAccess : public Errors::SimulationError {
			public:

				explicit BadGiftAccess(const std::string &message)
					: Errors::SimulationError(message) {}
		};
		template <typename T> class UniqueGift;

		/**
		 * @brief Provides exclusive temporary access to an owned object.
		 *
		 * @tparam T Type of the owned object.
		 *
		 * @details
		 * UniqueGift maintains ownership of the underlying object while
		 * allowing temporary exclusive access through GiftLease.
		 *
		 * Only one GiftLease may be active at a time.
		 *
		 * @warning
		 * UniqueGift must outlive every GiftLease obtained from it.
		 *
		 * @note
		 * This class provides runtime exclusivity, but does not prevent
		 * references or pointers from escaping the lifetime of a lease.
		 */
		template <typename T> class UniqueGift;

		template <typename T> class GiftLease {
				friend UniqueGift<T>;

			private:

				UniqueGift<T>       &original_gift;
				OptionalReference<T> ref;

				GiftLease(UniqueGift<T> &original, T &value)
					: original_gift(original)
					, ref(value) {}

			public:

				GiftLease(GiftLease &&other) noexcept
					: original_gift(other.original_gift)
					, ref(std::move(other.ref)) {}

				GiftLease(const GiftLease &)            = delete;
				GiftLease &operator=(const GiftLease &) = delete;
				GiftLease &operator=(GiftLease &&)      = delete;

				~GiftLease();

				T &get() {
					if (!ref.has_reference()) {
						throw BadGiftAccess("GiftLease::get() called on a lease that was already given back");
					}
					return ref.reference();
				}

				const T &get() const {
					if (!ref.has_reference()) {
						throw BadGiftAccess("GiftLease::get() called on a lease that was already given back");
					}
					return ref.reference();
				}

				bool has_gift() const {
					return ref.has_reference();
				}

				bool release();
		};

		template <typename T> class UniqueGift {
				friend GiftLease<T>;

			private:

				mutable std::mutex mutex;
				T                  value;

				std::atomic<bool> was_taken_ = false;

				void give_back(GiftLease<T> &gift_lease) {
					assert(&gift_lease.original_gift == this && "give_back called with lease from another gift");
					std::lock_guard lock{mutex};
					gift_lease.ref.reset();
					was_taken_ = false;
				}

			public:

				UniqueGift(const UniqueGift &)            = delete;
				UniqueGift &operator=(const UniqueGift &) = delete;
				UniqueGift(UniqueGift &&)                 = delete;
				UniqueGift &operator=(UniqueGift &&)      = delete;

				UniqueGift(T &&value)
					: value(std::move(value)) {}

				~UniqueGift() {
					if (was_taken_.load(std::memory_order_relaxed)) {
						terminate("Tried to delete an UniqueGift while it was taken");
					}
				}

				bool was_taken() const noexcept {
					std::lock_guard lock{mutex};
					return was_taken_;
				}

				GiftLease<T> take() {
					{
						std::lock_guard lock{mutex};

						if (was_taken_) {
							throw BadGiftAccess("Cannot take a gift if it is already taken");
						}
						was_taken_ = true;
					}
					return GiftLease<T>{*this, value};
				}
		};

		template <typename T> GiftLease<T>::~GiftLease() {
			if (ref.has_reference()) {
				original_gift.give_back(*this);
			}
		}

		template <typename T> bool GiftLease<T>::release() {
			if (ref.has_reference()) {
				original_gift.give_back(*this);
				return true;
			}
			return false;
		}
	} // namespace Functional
} // namespace KShared
