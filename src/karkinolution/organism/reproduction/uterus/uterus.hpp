#pragma once
#include <cstdint>
#include <karkinolution/math/stats/runtime_values.hpp>
#include <karkinolution/organism/reproduction/gestation.hpp>
#include <karkinolution/organism/stats.hpp>
#include <variant>
#include <vector>


using BornCount = IntegerLimited<uint8_t>;
using OrganismStats::Energy;
using OrganismStats::Life;
using OrganismStats::Health::Health;

struct PregnantUterus {
		std::vector<uint64_t> embryos;
		Gestation             gestation;
		BornCount             born_count;

		int  embryos_count() const;
		int  dead_embryos_count() const;
		bool has_embryos() const;
		bool all_children_borned() const;
		bool is_full() const;
};

struct EmptyUterus {};

using UterusState = std::variant<PregnantUterus, EmptyUterus>;

class Uterus {
	public:

		UterusState state = EmptyUterus{};

		Uterus(const UterusState &state, const Health &health, const Energy &energy)
			: state(state)
			, health(health)
			, energy(energy) {}

		Health health;
		Energy energy;

		NormalizedValue<float> hungry() const;

		float pregnancy_cost() const;

		bool is_pregnant() const;
		bool is_empty() const;


		PregnantUterus       &get_pregnant_uterus();
		const PregnantUterus &get_pregnant_uterus() const;
};
