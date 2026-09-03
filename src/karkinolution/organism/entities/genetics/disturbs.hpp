#pragma once
#include <cmath>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/utils/k_random.hpp>

using PhysicsStats::Efficiency;

using Genomes::Relation;
using Genomes::Transformations;

namespace GeneticDisturbs {
// Scramble base values - use either max_base or min_base, not both, to avoid inconsistent behavior
// (e.g., 1.05f with 0.70f)
inline constexpr float LOW_SCRAMBLE_MAX_BASE      = 1.05f;
inline constexpr float LOW_SCRAMBLE_MIN_BASE      = 0.95f;
inline constexpr float HIGH_DISTANCE_MULTIPLIER   = 1.0f;
inline constexpr float MEDIUM_DISTANCE_MULTIPLIER = 0.7f;
inline constexpr float LOW_DISTANCE_MULTIPLIER    = 0.4f;

struct ScrambleContext {
		float           max_base, min_base;
		float           distance_multiplier;
		float           inheritance_bias = 1.0f / 2.0f;
		int             mutations_counts = 1;
		SuppressContext suppress_context = SuppressContext{.chance_of_v1_being_suppressed = 0.28f,
														   .chance_of_v2_being_suppressed = 0.28f,
														   .suppressed_multiplier         = 0.6f};
};

template <std::integral T> struct TimeScrambleContext {
		T   max_base, min_base;
		int mutation_counts;
};

namespace StandardContexts {
inline constexpr auto transformations_scramble =
	ScrambleContext{.max_base            = 1.10f,
					.min_base            = 0.90f,
					.distance_multiplier = 0.4f,
					.inheritance_bias    = 1.0f / 2.0f,
					.mutations_counts    = 1,
					.suppress_context    = SuppressContext{
						   .chance_of_v1_being_suppressed = NormalizedValue<float>{0.2},
						   .chance_of_v2_being_suppressed = NormalizedValue<float>{0.2},
						   .suppressed_multiplier         = 0.4}};

inline constexpr auto morphology_scramble =
	ScrambleContext{.max_base            = 1.15f,
					.min_base            = 0.85f,
					.distance_multiplier = 0.4f,
					.inheritance_bias    = 1.0f / 2.0f,
					.mutations_counts    = 1,
					.suppress_context    = SuppressContext{
						   .chance_of_v1_being_suppressed = NormalizedValue<float>{0.2},
						   .chance_of_v2_being_suppressed = NormalizedValue<float>{0.2},
						   .suppressed_multiplier         = 0.4}};

inline constexpr auto metabolism_growth_rates =
	ScrambleContext{.max_base = 0.97f, .min_base = 1.03f, .distance_multiplier = 0.6f};
} // namespace StandardContexts

template <typename T, typename U> float scramble(T e_v1, U e_v2, const ScrambleContext &context) {
	float v1 = static_cast<float>(e_v1);
	float v2 = static_cast<float>(e_v2);

	RandomGenerators::suppress_one(v1, v2, context.suppress_context);

	float inheritance_factor = (v1 + v2) / 2.0f;
	float distance           = std::abs(v1 - v2) * context.distance_multiplier;

	float value =
		inheritance_factor * RandomGenerators::generate(context.min_base, context.max_base)
		+ RandomGenerators::generate(-distance, distance);

	for (int i = 0; i < context.mutations_counts; i++) {
		value *= RandomGenerators::generate(context.min_base, context.max_base);
	}
	return value;
}

template <std::integral T> T time_scramble(T v1, T v2, const TimeScrambleContext<T> &context) {
	T value = 0;

	for (int i = 0; i < context.mutation_counts; i++) {
		value += RandomGenerators::generate(context.min_base, context.max_base);
	}
	return value;
}

// Scramble transformations


template <typename Trait, typename F>
	requires std::invocable<F, Efficiency, Efficiency>
	&& std::same_as<std::invoke_result_t<F, Efficiency, Efficiency>, Efficiency>
Transformations<Trait> mix_transformations(const Transformations<Trait> &lhs,
										   const Transformations<Trait> &rhs,
										   F                           &&function) {
	Transformations<Trait> result;

	for (const auto &[resource, lhs_relations] : lhs.view()) {
		const auto &rhs_relations = rhs.at(resource);

		for (const auto &lhs_relation : lhs_relations) {
			const auto rhs_it = std::find_if(rhs_relations.begin(),
											 rhs_relations.end(),
											 [&](const Relation<Trait> &rhs_relation) {
												 return rhs_relation.target == lhs_relation.target;
											 });

			if (rhs_it == rhs_relations.end()) {
				throw std::out_of_range("Matching transformation not found");
			}

			Relation<Trait> relation = lhs_relation;

			relation.efficiency =
				std::invoke(std::forward<F>(function), lhs_relation.efficiency, rhs_it->efficiency);

			result.add(resource, std::move(relation));
		}
	}

	return result;
}
} // namespace GeneticDisturbs