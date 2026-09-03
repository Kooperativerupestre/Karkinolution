#pragma once

#include "karkinolution/math/stats/compile_values.hpp"

#include <concepts>
#include <cstdint>
#include <karkinolution/core/basestorage.hpp>
#include <karkinolution/core/id_generator.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/embryo/embryo.hpp>
#include <karkinolution/organism/entities/identity.hpp>
#include <karkinolution/organism/pathogens/identity.hpp>
#include <karkinolution/organism/pathogens/pathogen.hpp>
#include <karkinolution/organism/stats.hpp>
#include <variant>

enum class VirusSpecies {
	MORBUS_EXIGUUS,
	VORAX
};


using OrganismStats::Health::Immunity;

using Damage  = float;
using Consume = NormalizedValue<float>;
using PhysicsStats::Efficiency;

class ViralLoad : public NormalizedValue<float> {
		using NormalizedValue<float>::NormalizedValue;
};

struct VirusId {
		VirusSpecies  specie;
		PathogenTypes type;
		uint64_t      id;
};

class MorbusExiguus {
	public:

		VirusId    id;
		ViralLoad  viral_load;
		Efficiency efficiency; // size degree per tick
};

class Vorax {
	public:

		VirusId   id;
		ViralLoad viral_load;
		Consume   consume_metabolism;
		Consume   consume_muscles;
};

// Individual virus validation
template <typename T>
concept VirusType = requires(T virus) {
	requires std::same_as<decltype(virus.id), VirusId>;
	requires std::same_as<decltype(virus.viral_load), ViralLoad>;
};


// Variant validation
template <typename Variant> struct AllTypesSatisfyVirusType;

template <typename... Ts> struct AllTypesSatisfyVirusType<std::variant<Ts...>> {
		static constexpr bool value = (VirusType<Ts> && ...);
};

using AllVirusType = std::variant<Vorax, MorbusExiguus>;

static_assert(AllTypesSatisfyVirusType<AllVirusType>::value);

static_assert(VirusType<Vorax>);
static_assert(VirusType<MorbusExiguus>);

namespace VirusUtils {
	VirusId get_id(const AllVirusType &virus);

	template <typename T> decltype(auto) get_viral_load(T &&virus) {
		return std::visit(
			[](auto &&v) -> decltype(auto) {
				return std::forward<decltype(v)>(v).viral_load;
			},
			std::forward<T>(virus));
	}
} // namespace VirusUtils

class GenericVirus {
	public:

		AllVirusType state;

		VirusId id() const;
};

class VirusContainer : public PathogenContainer<GenericVirus, VirusId> {};

struct MetamorphosisEnvironment {
		Immunity   immunity;
		Efficiency efficiency;

		[[nodiscard]] NormalizedValue<float> immunity_factor() const noexcept;
};

namespace VirusIDF {
	inline VirusId create_virus_id(uint64_t id, VirusSpecies specie) {
		return VirusId{.specie = specie, .type = PathogenTypes::VIRUS, .id = id};
	}

	inline VirusId create_virus_id(VirusSpecies specie) {
		return VirusId{.specie = specie, .type = PathogenTypes::VIRUS, .id = gen_id()};
	}
} // namespace VirusIDF