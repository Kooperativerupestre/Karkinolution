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
					std::format("Invalid RuntimeLimitedValue range. Min() > Max()", _min, _max));
			}

			clamp();
		}

		T value() const noexcept {
			return _value;
		}

		T min() const noexcept {
			return _min;
		}

		T max() const noexcept {
			return _max;
		}

		T &max_ref() noexcept {
			return _max;
		}

		T &min_ref() noexcept {
			return _min;
		}

		void clamp() noexcept {
			if (_value < _min) {
				_value = _min;
			} else if (_value > _max) {
				_value = _max;
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

		template <typename U> Derived &operator-=(U other) {
			_value -= static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename U> Derived &operator*=(U other) {
			_value *= static_cast<T>(other);
			clamp();
			return derived();
		}

		template <typename U> Derived &operator/=(U other) {
			_value /= static_cast<T>(other);
			clamp();
			return derived();
		}

		Derived &zero() {
			_value = _min;
			return derived();
		}

		Derived &full() {
			_value = _max;
			return derived();
		}

		bool is_zero() const {
			return _value == Approx<T>(_min);
		}

		bool is_full() const {
			return _value == Approx<T>(_max);
		}

		NormalizedValue<T> ratio() const {
			if (_max == _min) {
				return NormalizedValue<T>(0);
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

		IntegerWithMax(T max)
			: _value(T(0))
			, _max(max) {
			if (_max < T(0)) {
				throw std::invalid_argument("Invalid IntegerWithMax");
			}
		}

		auto operator<=>(const IntegerWithMax &) const = default;

		void pass() {
			if (_value < _max)
				++_value;

			clamp();
		}

		NormalizedValue<float> ratio() const {
			if (_max == T(0))
				return NormalizedValue<float>(0.0f);

			return NormalizedValue<float>(static_cast<float>(_value) / static_cast<float>(_max));
		}

		T value() const {
			return _value;
		}

		T max() const {
			return _max;
		}

		void clamp() {
			if (_value < T(0))
				_value = T(0);
			else if (_value > _max)
				_value = _max;
		}

		void full() {
			_value = _max;
		}

		void zero() {
			_value = T(0);
		}

		T remaining_to_max() const {
			if (_value >= _max)
				return T(0);

			return _max - _value;
		}

		bool is_above_max() const {
			return _value > _max;
		}

		bool is_below_max() const {
			return _value < _max;
		}

		bool is_at_point() const {
			return _value == _max;
		}

		bool is_zero() const {
			return _value == T(0);
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

		IntegerLimited(T max)
			: _value(0)
			, _max(max) {
			if (_max < T(0)) {
				throw std::invalid_argument("Invalid IntegerLimited");
			}
		}

		auto operator<=>(const IntegerLimited &) const = default;

		void pass() {
			if (_value < _max)
				++_value;

			clamp();
		}

		NormalizedValue<float> ratio() const {
			if (_max == 0) {
				return NormalizedValue<float>(0.0f);
			}

			return NormalizedValue<float>(static_cast<float>(_value) / static_cast<float>(_max));
		}

		T value() const {
			return _value;
		}

		T max() const {
			return _max;
		}

		void clamp() {
			if (_value < T(0))
				_value = T(0);
			else if (_value > _max)
				_value = _max;
		}

		void full() {
			_value = _max;
		}

		void zero() {
			_value = T(0);
		}

		T remaining_to_max() const {
			return std::max(T(0), _max - _value);
		}

		bool is_below_max() const {
			return _value < _max;
		}

		bool is_full() const {
			return _value == Approx<T>(_max);
		}

		bool is_zero() const {
			return _value == Approx<T>(T(0));
		}
};

// Generic runtime value
template <typename T> class RuntimeFactor : public RuntimeLimitedValue<RuntimeFactor<T>, T> {
		using RuntimeLimitedValue<RuntimeFactor, T>::RuntimeLimitedValue;
};
