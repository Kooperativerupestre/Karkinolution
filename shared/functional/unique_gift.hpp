#pragma once
#include "shared/core/error.hpp"
#include "shared/functional/optional_reference.hpp"

#include <mutex>

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
					return ref.reference();
				}

				const T &get() const {
					return ref.reference();
				}
		};

		template <typename T> class UniqueGift {
				friend GiftLease<T>;

			private:

				mutable std::mutex mutex;
				T                  value;

				bool was_taken_ = false;

			public:

				UniqueGift(const UniqueGift &)            = delete;
				UniqueGift &operator=(const UniqueGift &) = delete;
				UniqueGift(UniqueGift &&)                 = delete;
				UniqueGift &operator=(UniqueGift &&)      = delete;

				UniqueGift(T &&value)
					: value(std::move(value)) {}

				void give_back(GiftLease<T> &gift_lease) {
					std::lock_guard lock{mutex};
					gift_lease.ref.reset();
					was_taken_ = false;
				}

				~UniqueGift() noexcept {
					if (was_taken_) {
						std::terminate();
					}
				}

				bool was_taken() const noexcept {
					std::lock_guard lock{mutex};
					return was_taken_;
				}

				GiftLease<T> take() {
					std::lock_guard lock{mutex};

					if (was_taken_) {
						throw BadGiftAccess("Cannot take a gift if it is already taken");
					}
					was_taken_ = true;
					return GiftLease<T>{*this, value};
				}
		};

		template <typename T> GiftLease<T>::~GiftLease() {
			if (ref.has_reference()) {
				original_gift.give_back(*this);
			}
		}
	} // namespace Functional
} // namespace KShared
