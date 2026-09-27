#pragma once
#include <concepts>

template <typename T>
concept LimitedValueLike = requires(const T value) {
	typename T::value_type;

	{ value.value() } -> std::same_as<typename T::value_type>;
	{ value.max } -> std::same_as<const typename T::value_type &>;
	{ value.min } -> std::same_as<const typename T::value_type &>;
};

template <typename T>
concept RuntimeLimitedValueLike = requires(T value) {
	typename T::value_type;

	{ value.max() } -> std::same_as<typename T::value_type>;
	{ value.min() } -> std::same_as<typename T::value_type>;
	{ value.value() } -> std::same_as<typename T::value_type>;
};

template <typename T>
concept StatLike = LimitedValueLike<T> || RuntimeLimitedValueLike<T>;