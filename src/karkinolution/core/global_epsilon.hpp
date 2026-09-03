#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <concepts>
#include <limits>

/*
 * I decided to adopt a stricter (and more precise) policy regarding
 * number rounding, since floating-point precision could no longer be
 * trusted as the project's default — as it was in Python.
 *
 * This is also meant to prevent specific comparison and equality
 * errors that will be necessary when I have to perform specific
 * rate, growth, etc. calculations for the creatures.
 */

template <std::floating_point T> class Approx {
	public:

		constexpr explicit Approx(T value,
								  T epsilon_abs = std::numeric_limits<T>::epsilon() * 10,
								  T epsilon_rel = std::numeric_limits<T>::epsilon() * 10)
			: value_(value)
			, epsilon_abs_(epsilon_abs)
			, epsilon_rel_(epsilon_rel) {
			assert(std::isfinite(value_) && "Approx: Nan or infinite value received");
		}

		// == and != -------------------------------------------------------------

		constexpr friend bool operator==(T lhs, const Approx &rhs) {
			assert(std::isfinite(lhs) && "Approx==: Left side is Nan or infinite");

			T diff = std::abs(lhs - rhs.value_);
			if (diff <= rhs.epsilon_abs_) {
				return true;
			}
			return diff <= rhs.epsilon_rel_ * std::max(std::abs(lhs), std::abs(rhs.value_));
		}

		constexpr friend bool operator==(const Approx &lhs, T rhs) {
			return rhs == lhs;
		}

		constexpr friend bool operator!=(T lhs, const Approx &rhs) {
			return !(lhs == rhs);
		}

		constexpr friend bool operator!=(const Approx &lhs, T rhs) {
			return !(rhs == lhs);
		}

		// < and >

		constexpr friend bool operator<(T lhs, const Approx &rhs) {
			return lhs < rhs.value_ && !(lhs == rhs);
		}

		constexpr friend bool operator<(const Approx &lhs, T rhs) {
			return lhs.value_ < rhs && !(lhs == rhs);
		}

		constexpr friend bool operator>(T lhs, const Approx &rhs) {
			return lhs > rhs.value_ && !(lhs == rhs);
		}

		constexpr friend bool operator>(const Approx &lhs, T rhs) {
			return lhs.value_ > rhs && !(lhs == rhs);
		}

		// <= and >=

		constexpr friend bool operator<=(T lhs, const Approx &rhs) {
			return lhs < rhs.value_ || (lhs == rhs);
		}

		constexpr friend bool operator<=(const Approx &lhs, T rhs) {
			return lhs.value_ < rhs || (lhs == rhs);
		}

		constexpr friend bool operator>=(T lhs, const Approx &rhs) {
			return lhs > rhs.value_ || (lhs == rhs);
		}

		constexpr friend bool operator>=(const Approx &lhs, T rhs) {
			return lhs.value_ > rhs || (lhs == rhs);
		}

	private:

		T value_;
		T epsilon_abs_;
		T epsilon_rel_;
};