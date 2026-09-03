#pragma once
#include <compare>
#include <karkinolution/core/global_epsilon.hpp>
#include <stdexcept>


/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

template <typename T> class NormalizedValue;

template <typename T> class SignedNormalizedValue;

template <typename Derived, typename T, T max, T min> class LimitedValue {
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
			if (min > Approx<T>(max)) {
				throw std::invalid_argument("Invalid LimitedValue range");
			}

			clamp();
		}

		constexpr operator T() const {
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
			result.clamp();

			return result;
		}

		template <typename U> Derived operator+(U other) const {
			Derived result = derived();
			result += other;
			result.clamp();

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
			result.clamp();

			return result;
		}

		template <typename U> constexpr Derived operator-(U other) const {
			Derived result = derived();
			result -= other;
			result.clamp();

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
			result.clamp();
			return result;
		}

		template <typename U> constexpr Derived &operator/=(U other) {
			_value /= static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename U> constexpr Derived operator/(U other) {
			Derived result = derived();
			result /= other;
			result.clamp();
			return result;
		}

		constexpr auto operator<=>(const LimitedValue<Derived, T, max, min> &) const = default;

		void zero() {
			_value = min;
			clamp();
		}

		void full() {
			_value = max;
			clamp();
		}

		NormalizedValue<T> ratio() const;

		SignedNormalizedValue<T> ratio_min() const;

		bool is_full() const {
			return value() == Approx<T>(max());
		}

		bool is_zero() const {
			return value() == Approx<T>(T(0));
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
NormalizedValue<T> LimitedValue<Derived, T, max, min>::ratio() const {
	return NormalizedValue<T>(static_cast<T>(_value - min) / static_cast<T>(max - min));
}

template <typename Derived, typename T, T max, T min>
SignedNormalizedValue<T> LimitedValue<Derived, T, max, min>::ratio_min() const {
	return SignedNormalizedValue<T>(static_cast<T>(_value - min) / static_cast<T>(max - min) * T(2)
									- T(1));
}

// Generic LimitedValue

template <typename T, T max, T min>
class Factor : public LimitedValue<Factor<T, max, min>, T, max, min> {
		using LimitedValue<Factor, T, max, min>::LimitedValue;
};
