#pragma once
#include <karkinolution/core/global_epsilon.hpp>


/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

template <typename T> class NormalizedValue;

template <typename T> class SignedNormalizedValue;

template <typename Derived, typename T, T max, T min> class LimitedValue {
		static_assert(min <= max, "min must be less than or equal to max");

	private:

		T _value;

	protected:

		Derived &derived() {
			return static_cast<Derived &>(*this);
		}

		const Derived &derived() const {
			return static_cast<const Derived &>(*this);
		}

	public:

		constexpr LimitedValue(T value = T(0))
			: _value(value) {
			clamp();
		}

		constexpr explicit operator T() const {
			return value();
		}

		constexpr T value() const {
			return _value;
		}

		constexpr void clamp() {
			if (_value < min) {
				_value = min;
			} else if (_value > max) {
				_value = max;
			}
		}

		template <typename U> Derived &set(U new_value) {
			_value = static_cast<T>(new_value);
			clamp();
			return derived();
		}

		template <typename U> Derived &operator+=(U other) {
			_value += static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename OtherDerived, typename U, U other_max, U other_min>
		Derived operator+(const LimitedValue<OtherDerived, U, other_max, other_min> &other) const {
			Derived result = derived();
			result += other.value();

			return result;
		}

		template <typename U> Derived operator+(U other) const {
			Derived result = derived();
			result += other;

			return result;
		}

		template <typename U> Derived &operator-=(U other) {
			_value -= static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename OtherDerived, typename U, U other_max, U other_min>
		Derived operator-(const LimitedValue<OtherDerived, U, other_max, other_min> &other) const {
			Derived result = derived();
			result -= other.value();

			return result;
		}

		template <typename U> constexpr Derived operator-(U other) const {
			Derived result = derived();
			result -= other;

			return result;
		}

		template <typename U> constexpr Derived &operator*=(U other) {
			_value *= static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename U> constexpr Derived operator*(U other) const {
			Derived result = derived();
			result *= other;
			return result;
		}

		template <typename U> constexpr Derived &operator/=(U other) {
			_value /= static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename U> constexpr Derived operator/(U other) const {
			Derived result = derived();
			result /= other;
			return result;
		}

		constexpr bool operator==(const LimitedValue &other) const {
			if constexpr (std::floating_point<T>) {
				return value() == Approx<T>(other.value());
			} else {
				return value() == other.value();
			}
		}

		constexpr bool operator!=(const LimitedValue &other) const {
			return !(*this == other);
		}

		constexpr bool operator<(const LimitedValue &other) const {
			if constexpr (std::floating_point<T>) {
				return value() < Approx<T>(other.value());
			} else {
				return value() < other.value();
			}
		}

		constexpr bool operator>(const LimitedValue &other) const {
			if constexpr (std::floating_point<T>) {
				return value() > Approx<T>(other.value());
			} else {
				return value() > other.value();
			}
		}

		constexpr bool operator<=(const LimitedValue &other) const {
			if constexpr (std::floating_point<T>) {
				return value() <= Approx<T>(other.value());
			} else {
				return value() <= other.value();
			}
		}

		constexpr bool operator>=(const LimitedValue &other) const {
			if constexpr (std::floating_point<T>) {
				return value() >= Approx<T>(other.value());
			} else {
				return value() >= other.value();
			}
		}

		void zero() {
			static_assert(min <= T(0), "Min should accept zero to zero the value on LimitedValue");

			_value = T(0);
			clamp();
		}

		void full() {
			_value = max;
			clamp();
		}

		constexpr T remaining() const {
			return max - _value;
		}

		template <typename D = Derived>
			requires(!std::same_as<D, NormalizedValue<T>>
					 && !std::same_as<D, SignedNormalizedValue<T>>)
		NormalizedValue<T> ratio() const;

		template <typename D = Derived>
			requires(!std::same_as<D, NormalizedValue<T>>
					 && !std::same_as<D, SignedNormalizedValue<T>>)
		SignedNormalizedValue<T> ratio_min() const;

		bool is_full() const {
			if constexpr (std::floating_point<T>) {
				return value() == Approx<T>(max);
			} else {
				return value() == max;
			}
		}

		bool is_zero() const {
			if constexpr (std::floating_point<T>) {
				return value() == Approx<T>(T(0));
			} else {
				return value() == T(0);
			}
		}
};

template <typename T>
class NormalizedValue : public LimitedValue<NormalizedValue<T>, T, T(1), T(0)> {
	public:

		constexpr NormalizedValue(T value = T(0))
			: LimitedValue<NormalizedValue, T, T(1), T(0)>(value) {}
};

template <typename T>
class SignedNormalizedValue : public LimitedValue<SignedNormalizedValue<T>, T, T(1), T(-1)> {
	public:

		constexpr SignedNormalizedValue(T value = T(0))
			: LimitedValue<SignedNormalizedValue, T, T(1), T(-1)>(value) {}
};

template <typename Derived, typename T, T max, T min>
template <typename D>
	requires(!std::same_as<D, NormalizedValue<T>> && !std::same_as<D, SignedNormalizedValue<T>>)
NormalizedValue<T> LimitedValue<Derived, T, max, min>::ratio() const {
	return NormalizedValue<T>(static_cast<T>(_value - min) / static_cast<T>(max - min));
}

template <typename Derived, typename T, T max, T min>
template <typename D>
	requires(!std::same_as<D, NormalizedValue<T>> && !std::same_as<D, SignedNormalizedValue<T>>)
SignedNormalizedValue<T> LimitedValue<Derived, T, max, min>::ratio_min() const {
	return SignedNormalizedValue<T>(static_cast<T>(_value - min) / static_cast<T>(max - min) * T(2)
									- T(1));
}

template <typename T, T max, T min>
class GenericLimitedValue : public LimitedValue<GenericLimitedValue<T, max, min>, T, max, min> {

		using Base = LimitedValue<GenericLimitedValue<T, max, min>, T, max, min>;

	public:

		using Base::Base;
};