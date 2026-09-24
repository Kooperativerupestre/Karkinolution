#pragma once
#include <format>
#include <karkinolution/math/stats/compile_values.hpp>

template <typename Derived, typename T> class RuntimeLimitedValue {
	private:

		T _value;
		T _min;
		T _max;

	protected:

		Derived &derived() {
			return static_cast<Derived &>(*this);
		}

		const Derived &derived() const {
			return static_cast<const Derived &>(*this);
		}

	public:

		RuntimeLimitedValue(T value, T max, T min = T(0))
			: _value(value)
			, _min(min)
			, _max(max) {
			if (_min > _max) {
				throw std::invalid_argument(
					std::format("Invalid RuntimeLimitedValue range. Min > Max: {} > {}",
								_min,
								_max));
			}

			if (_min > T(0)) {
				throw std::invalid_argument(
					std::format("Invalid RuntimeLimitedValue range. Min must accept zero: {}",
								_min));
			}

			clamp();
		}

		constexpr explicit operator T() const {
			return value();
		}

		constexpr T value() const noexcept {
			return _value;
		}

		constexpr T min() const noexcept {
			return _min;
		}

		constexpr T max() const noexcept {
			return _max;
		}

		constexpr T &max_ref() noexcept {
			return _max;
		}

		constexpr T &min_ref() noexcept {
			return _min;
		}

		constexpr void clamp() noexcept {
			if (_value < _min) {
				_value = _min;
			} else if (_value > _max) {
				_value = _max;
			}
		}

		template <typename U> constexpr Derived &set(U new_value) {
			_value = static_cast<T>(new_value);
			clamp();
			return derived();
		}

		template <typename U> constexpr Derived &operator+=(U other) {
			_value += static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename U> constexpr Derived &operator-=(U other) {
			_value -= static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename U> constexpr Derived &operator*=(U other) {
			_value *= static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename U> constexpr Derived &operator/=(U other) {
			_value /= static_cast<T>(other);
			clamp();
			return derived();
		}

		Derived &zero() {
			_value = T(0);
			clamp();
			return derived();
		}

		Derived &full() {
			_value = _max;
			return derived();
		}

		constexpr bool is_zero() const {
			if constexpr (std::floating_point<T>) {
				return _value == Approx<T>(T(0));
			} else {
				return _value == T(0);
			}
		}

		constexpr bool is_full() const {
			if constexpr (std::floating_point<T>) {
				return _value == Approx<T>(_max);
			} else {
				return _value == _max;
			}
		}

		constexpr T remaining() const {
			return _max - _value;
		}

		NormalizedValue<T> ratio() const {
			if (_max == _min) {
				return NormalizedValue<T>(T(0));
			}

			return NormalizedValue<T>((_value - _min) / (_max - _min));
		}
};

template <typename T> class IntegerWithMax {
	private:

		T _value;
		T _max;

	public:

		IntegerWithMax(T value, T max)
			: _value(value)
			, _max(max) {
			if (_max < T(0)) {
				throw std::invalid_argument("Invalid IntegerWithMax");
			}

			clamp();
		}

		explicit IntegerWithMax(T max)
			: _value(T(0))
			, _max(max) {
			if (_max < T(0)) {
				throw std::invalid_argument("Invalid IntegerWithMax");
			}
		}

		constexpr explicit operator T() const noexcept {
			return value();
		}

		constexpr T value() const noexcept {
			return _value;
		}

		constexpr T max() const noexcept {
			return _max;
		}

		constexpr void clamp() noexcept {
			if (_value < T(0)) {
				_value = T(0);
			} else if (_value > _max) {
				_value = _max;
			}
		}

		void pass() {
			if (_value < _max) {
				++_value;
			}
		}

		void zero() noexcept {
			_value = T(0);
		}

		void full() noexcept {
			_value = _max;
		}

		constexpr T remaining_to_max() const noexcept {
			return _max - _value;
		}

		constexpr bool is_above_max() const noexcept {
			return _value > _max;
		}

		constexpr bool is_below_max() const noexcept {
			return _value < _max;
		}

		constexpr bool is_full() const noexcept {
			return _value == _max;
		}

		constexpr bool is_zero() const noexcept {
			return _value == T(0);
		}

		NormalizedValue<float> ratio() const {
			if (_max == T(0)) {
				return NormalizedValue<float>(0.0f);
			}

			return NormalizedValue<float>(static_cast<float>(_value) / static_cast<float>(_max));
		}
};

template <typename T> class IntegerLimited {
	private:

		T _value;
		T _max;

	public:

		IntegerLimited(T value, T max)
			: _value(value)
			, _max(max) {
			if (_max < T(0)) {
				throw std::invalid_argument("Invalid IntegerLimited");
			}

			clamp();
		}

		explicit IntegerLimited(T max)
			: _value(T(0))
			, _max(max) {
			if (_max < T(0)) {
				throw std::invalid_argument("Invalid IntegerLimited");
			}
		}

		constexpr explicit operator T() const noexcept {
			return value();
		}

		constexpr T value() const noexcept {
			return _value;
		}

		constexpr T max() const noexcept {
			return _max;
		}

		constexpr void clamp() noexcept {
			if (_value < T(0)) {
				_value = T(0);
			} else if (_value > _max) {
				_value = _max;
			}
		}

		void pass() {
			if (_value < _max) {
				++_value;
			}
		}

		void zero() noexcept {
			_value = T(0);
		}

		void full() noexcept {
			_value = _max;
		}

		constexpr T remaining_to_max() const noexcept {
			return std::max(T(0), _max - _value);
		}

		constexpr bool is_below_max() const noexcept {
			return _value < _max;
		}

		constexpr bool is_full() const noexcept {
			return _value == _max;
		}

		constexpr bool is_zero() const noexcept {
			return _value == T(0);
		}

		NormalizedValue<float> ratio() const {
			if (_max == T(0)) {
				return NormalizedValue<float>(0.0f);
			}

			return NormalizedValue<float>(static_cast<float>(_value) / static_cast<float>(_max));
		}
};

template <typename T>
class GenericRuntimeValue : public RuntimeLimitedValue<GenericRuntimeValue<T>, T> {

		using Base = RuntimeLimitedValue<GenericRuntimeValue<T>, T>;

	public:

		using Base::Base;
};