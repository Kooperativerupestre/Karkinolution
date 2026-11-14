#pragma once

#include "shared/core/error.hpp"

#include <concepts>
#include <memory>
#include <utility>

namespace KShared {
	namespace Functional {
		class BadOptionalReferenceAccessError : public Errors::SimulationError {
			public:

				explicit BadOptionalReferenceAccessError(const std::string &message)
					: Errors::SimulationError(message) {}
		};

		template <typename T> class OptionalReference {
				static_assert(!std::is_reference_v<T>);

			private:

				T* ptr = nullptr;

			public:

				// Constructors
				constexpr OptionalReference() noexcept {
					ptr = nullptr;
				}

				constexpr OptionalReference(std::nullptr_t) noexcept {
					ptr = nullptr;
				}

				constexpr OptionalReference(T &value) noexcept
					: ptr(std::addressof(value)) {}

				OptionalReference(T &&) = delete;

				constexpr OptionalReference(const OptionalReference &) noexcept = default;

				constexpr OptionalReference(OptionalReference &&other) noexcept
					: ptr(std::exchange(other.ptr, nullptr)) {}

				template <typename U>
					requires std::convertible_to<U*, T*>
				constexpr OptionalReference(const OptionalReference<U> &other) noexcept
					: ptr(other.get()) {}

				// Assignment
				constexpr OptionalReference &operator=(const OptionalReference &) noexcept = default;

				constexpr OptionalReference &operator=(OptionalReference &&other) noexcept {
					if (this != &other) {
						ptr = std::exchange(other.ptr, nullptr);
					}
					return *this;
				}

				constexpr OptionalReference &operator=(std::nullptr_t) noexcept {
					reset();
					return *this;
				}

				constexpr OptionalReference &operator=(T &value) noexcept {
					ptr = std::addressof(value);
					return *this;
				}

				template <typename U>
					requires std::convertible_to<U*, T*>
				constexpr OptionalReference &operator=(const OptionalReference<U> &other) noexcept {
					ptr = other.get();
					return *this;
				}

				// Observers
				[[nodiscard]]
				constexpr bool has_reference() const noexcept {
					return ptr != nullptr;
				}

				[[nodiscard]]
				constexpr explicit operator bool() const noexcept {
					return has_reference();
				}

				[[nodiscard]]
				constexpr T* get() const noexcept {
					return ptr;
				}

				// Accessors
				[[nodiscard]]
				constexpr T &reference() const {
					if (!has_reference()) {
						throw BadOptionalReferenceAccessError(
							"OptionalReference::reference() called on an empty OptionalReference");
					}
					return *ptr;
				}

				[[nodiscard]]
				constexpr T &operator*() const noexcept {
					assert(has_reference() && "OptionalReference::operator* called on an empty OptionalReference");
					return *ptr;
				}

				[[nodiscard]]
				constexpr T* operator->() const noexcept {
					assert(has_reference() && "OptionalReference::operator-> called on an empty OptionalReference");
					return ptr;
				}

				constexpr T &reference_or(T &fallback) const noexcept {
					return has_reference() ? *ptr : fallback;
				}

				// Modifiers
				constexpr void reset() noexcept {
					ptr = nullptr;
				}

				constexpr void swap(OptionalReference &other) noexcept {
					std::swap(ptr, other.ptr);
				}

				// Comparisons
				[[nodiscard]]
				constexpr bool operator==(std::nullptr_t) const noexcept {
					return !has_reference();
				}

				[[nodiscard]]
				constexpr bool operator!=(std::nullptr_t) const noexcept {
					return has_reference();
				}

				friend void swap(OptionalReference &a, OptionalReference &b) noexcept {
					a.swap(b);
				}

				friend constexpr bool operator==(std::nullptr_t, const OptionalReference &opt) noexcept {
					return !opt.has_reference();
				}

				template <typename U>
					requires std::convertible_to<U*, T*> || std::convertible_to<T*, U*>
				[[nodiscard]] bool has_same_reference(const OptionalReference<U> &opt_ref) const noexcept {
					return get() == opt_ref.get();
				}
		};
	} // namespace Functional
} // namespace KShared