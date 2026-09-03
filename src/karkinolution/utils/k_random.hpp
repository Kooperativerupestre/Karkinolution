#pragma once
#include <array>
#include <cmath>
#include <concepts>
#include <iterator>
#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <limits>
#include <random>
#include <type_traits>
#include <vector>

inline std::random_device rd;
inline std::mt19937_64    gen(rd());

namespace Choices {
template <std::ranges::random_access_range Range> decltype(auto) choice(Range &&range) {
	std::uniform_int_distribution<size_t> dist(0, std::ranges::size(range) - 1);
	return range[dist(gen)];
}

template <std::ranges::forward_range Range>
	requires(!std::ranges::random_access_range<Range> && std::ranges::sized_range<Range>)
decltype(auto) choice(Range &&range) {
	std::uniform_int_distribution<size_t> dist(0, std::ranges::size(range) - 1);

	auto it = std::ranges::begin(range);
	std::ranges::advance(it, dist(gen));

	return *it;
}

template <std::ranges::forward_range Range>
	requires(!std::ranges::random_access_range<Range> && !std::ranges::sized_range<Range>)
decltype(auto) choice(Range &&range) {
	auto size = std::ranges::distance(range);

	std::uniform_int_distribution<std::ranges::range_difference_t<Range>> dist(0, size - 1);

	auto it = std::ranges::begin(range);
	std::ranges::advance(it, dist(gen));

	return *it;
}

template <typename T> T choice(std::initializer_list<T> list) {
	std::uniform_int_distribution<size_t> dist(0, list.size() - 1);
	return *(list.begin() + dist(gen));
}

template <typename ItemsContainer, typename WeightsContainer>
auto choices(ItemsContainer &&items, WeightsContainer &&weights, size_t k) {
	using PlainItems = std::remove_reference_t<ItemsContainer>;
	using ItemType   = typename PlainItems::value_type;

	std::vector<ItemType> selected;
	selected.reserve(k);

	std::discrete_distribution<size_t> dist(std::begin(std::forward<WeightsContainer>(weights)),
											std::end(std::forward<WeightsContainer>(weights)));

	for (size_t i = 0; i < k; ++i) {
		size_t sampled_index = dist(gen);
		auto   it            = std::begin(std::forward<ItemsContainer>(items));
		std::advance(it, sampled_index);
		selected.push_back(*it);
	}

	return selected;
}

template <typename ItemsContainer> auto choices(const ItemsContainer &items, size_t k) {
	using ItemType = std::decay_t<decltype(*std::begin(items))>;

	size_t num_items = std::distance(std::begin(items), std::end(items));

	if (num_items == 0 || k == 0)
		return std::vector<ItemType>{};

	std::vector<double> uniform_weights(num_items, 1.0);
	return choices(items, uniform_weights, k);
}

template <typename T, typename U> bool choice_bool(T true_weight, U false_weight) {
	auto weights = {true_weight, false_weight};
	return Choices::choices(std::array<bool, 2>{true, false}, weights, 1)[0];
}

template <typename T> bool choice_bool(const std::array<T, 2> &weights) {
	return Choices::choices(std::array<bool, 2>{true, false}, weights, 1)[0];
}

template <typename T> bool choice_bool(T true_weight) {
	return Choices::choices(std::array<bool, 2>{true, false},
							std::array<T, 2>{true_weight, 1 - true_weight},
							1)[0];
}

template <typename T>
bool choice_bool(const NormalizedValue<float> &true_weight, const NormalizedValue<float> &v2) {
	return Choices::choices(std::array<bool, 2>{true, false},
							std::array<float, 2>{true_weight.value(), v2.value()},
							1)[0];
}

template <typename T> bool choice_bool(const NormalizedValue<float> &true_weight) {
	return Choices::choices(std::array<bool, 2>{true, false},
							std::array<float, 2>{true_weight.value(), 1.0f - true_weight.value()},
							1)[0];
}
} // namespace Choices

struct SuppressContext {
		NormalizedValue<float> chance_of_v1_being_suppressed;
		NormalizedValue<float> chance_of_v2_being_suppressed;

		float suppressed_multiplier;
};

enum class SupressOneOutput : uint8_t {
	V1_HAS_SUPRESSED,
	V2_HAS_SUPPRESSED,
	NOTHING_HAS_SUPPRESSED
};

enum class SuppressTwoOutput : uint8_t {
	V1_HAS_SUPRESSED,
	V2_HAS_SUPPRESSED,
	NOTHING_HAS_SUPPRESSED,
	ALL_VALUES_HAS_SUPRESSED
};

namespace RandomGenerators {
template <std::floating_point T> T generate(T min, T max) {
	return std::uniform_real_distribution<T>{min, max}(gen);
}

template <std::integral T> T generate(T min, T max) {
	return std::uniform_int_distribution<T>{min, max}(gen);
}

inline Vec3 generate(const Vec3 &min, const Vec3 &max) {
	return Vec3{generate(min.x, max.x), generate(min.y, max.y), generate(min.z, max.z)};
}

template <std::floating_point T> T generate() {
	const auto limit = std::numeric_limits<T>();
	return std::uniform_real_distribution<T>(limit.lowest(), limit.max())(gen);
}

template <std::integral T> T generate() {
	const auto limit = std::numeric_limits<T>();
	return std::uniform_int_distribution<T>(limit.lowest(), limit.max())(gen);
}

template <Vec3Like T> Vec3 generate() {
	return Vec3{generate<double>(), generate<double>(), generate<double>()};
}

template <typename T, typename U> float inheritance(T v1, U v2) {
	float bias = generate(0.0f, 1.0f);
	return v1 + (v2 - v1) * bias;
}

template <typename T, typename U> float inheritance(T v1, U v2, float bias) {
	return v1 + (v2 - v1) * bias;
}

template <typename T, typename U>
SupressOneOutput suppress_one(T &v1, U &v2, const SuppressContext &context) {
	if (Choices::choice_bool(context.chance_of_v1_being_suppressed)) {
		v1 *= context.suppressed_multiplier;
		return SupressOneOutput::V1_HAS_SUPRESSED;
	} else if (Choices::choice_bool(context.chance_of_v2_being_suppressed)) {
		v2 *= context.suppressed_multiplier;
		return SupressOneOutput::V2_HAS_SUPPRESSED;
	} else {
		return SupressOneOutput::NOTHING_HAS_SUPPRESSED;
	}
}

template <typename T, typename U>
SuppressTwoOutput suppress_two(T &v1, U &v2, const SuppressContext &context) {
	bool v1_has_suppressed = Choices::choice_bool(context.chance_of_v1_being_suppressed);
	bool v2_has_suppressed = Choices::choice_bool(context.chance_of_v2_being_suppressed);

	if (v1_has_suppressed) {
		v1 *= context.suppressed_multiplier;
	}

	if (v2_has_suppressed) {
		v2 *= context.suppressed_multiplier;
	}

	if (v1_has_suppressed && v2_has_suppressed) {
		return SuppressTwoOutput::ALL_VALUES_HAS_SUPRESSED;
	} else if (v1_has_suppressed) {
		return SuppressTwoOutput::V1_HAS_SUPRESSED;
	} else if (v2_has_suppressed) {
		return SuppressTwoOutput::V2_HAS_SUPPRESSED;
	} else {
		return SuppressTwoOutput::NOTHING_HAS_SUPPRESSED;
	}
}

} // namespace RandomGenerators