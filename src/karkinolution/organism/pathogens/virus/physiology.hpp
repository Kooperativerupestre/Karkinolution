#pragma once
#include <karkinolution/organism/pathogens/pathogen.hpp>
#include <karkinolution/organism/pathogens/virus/virus.hpp>
#include <karkinolution/organism/stats.hpp>

using OrganismStats::Health::Immunity;

namespace VirusPhysiology {
	template <VirusType T> bool is_dead(const T &virus) {
		return virus.viral_load.is_zero();
	}

	bool is_dead(const GenericVirus &virus);


	template <VirusType V>
	[[nodiscard]] ViralLoad calculate_min_viral_load(const V                        &virus,
													 const MetamorphosisEnvironment &environment);
	template <VirusType V>
	[[nodiscard]] ViralLoad calculate_max_viral_load(const V                        &virus,
													 const MetamorphosisEnvironment &environment);

	[[nodiscard]] Vorax         metamorphosis(const Vorax                    &virus,
											  const MetamorphosisEnvironment &environment);
	[[nodiscard]] MorbusExiguus metamorphosis(const MorbusExiguus            &virus,
											  const MetamorphosisEnvironment &environment);

} // namespace VirusPhysiology
