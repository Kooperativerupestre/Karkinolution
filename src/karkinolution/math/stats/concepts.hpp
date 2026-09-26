#pragma once
#include <concepts>

template <typename T>
concept LimitedValueLike = requires(T value) {
	{ value.max() } -> std::same_as<T>;
	{ value.min() } -> std::same_as<T>;
	{ value.value() } -> std::same_as<T>;
};

template <typename T>
concept RuntimeLimitedValueLike = requires(T value) {
	{ value.max() } -> std::same_as<T>;
	{ value.min() } -> std::same_as<T>;
	{ value.value() } -> std::same_as<T>;
};