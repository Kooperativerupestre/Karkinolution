#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <flat_map>
#include <karkinolution/functional/k_random.hpp>
#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>
#include <karkinolution/organism/reproduction/state/ontology.hpp>
#include <karkinolution/organism/values.hpp>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>

namespace Karkinolution {
	namespace Organisms {
		namespace Entities {
			namespace Genetic {

				enum class Resource : uint8_t {
					RESERVED_ENERGY
				};

				namespace CreatureGenomes {

					namespace Metabolism {

						struct GrowthRates {
								GrowthRate max_energy;
								GrowthRate energy;
								GrowthRate max_reserved_energy;
						};

						struct MetabolismGenome {
								float          max_energy;
								float          max_reserved_energy;
								Creature::Diet diet;
								GrowthRates    growth_rates;
						};

					} // namespace Metabolism

					// 3.2: Vital
					namespace Vital {

						struct GrowthRates {
								GrowthRate life;
						};

						struct VitalGenome {
								float       max_life;
								Immunity    average_immunity;
								Health      average_health;
								Age         average_age;
								GrowthRates growth_rates;
						};

					} // namespace Vital

					namespace Morphology {


						struct MorphologyGenome {
								Math::Units::Lateral average_lateral;
								Math::Units::Depth   average_back;
								Math::Units::Height  average_height;
								Mass                 average_mass;
						};

					} // namespace Morphology

					namespace Reproduction {

						struct ViviparousOrganismGenome {};

						struct OviparousOrganismGenome {};

						inline Karkinolution::Organisms::Entities::Reproduction::State::Ontology::
							ReproductiveWays
							get_reproductive_way(
								const std::variant<ViviparousOrganismGenome,
												   OviparousOrganismGenome> &genome) {
							if (std::holds_alternative<ViviparousOrganismGenome>(genome)) {
								return Karkinolution::Organisms::Entities::Reproduction::State::
									Ontology::ReproductiveWays::VIVIPAROUS;
							} else if (std::holds_alternative<OviparousOrganismGenome>(genome)) {
								return Karkinolution::Organisms::Entities::Reproduction::State::
									Ontology::ReproductiveWays::OVIPAROUS;
							}
							std::unreachable();
						}

						struct ReproductiveGenome {
								std::variant<ViviparousOrganismGenome, OviparousOrganismGenome>
													   reproductive_way_genome;
								int                    average_children_count;
								int                    average_gestation_limit;
								int                    fertility_limit;
								NormalizedValue<float> reproduction_cost;

								Karkinolution::Organisms::Entities::Reproduction::State::Ontology::
									ReproductiveWays
									reproductive_way() const noexcept {
									return get_reproductive_way(reproductive_way_genome);
								}
						};


					} // namespace Reproduction

				} // namespace CreatureGenomes

				namespace EmbryoGenomes {

					struct BodyGenome {
							NormalizedValue<float> average_max_energy;
							NormalizedValue<float> average_max_life;
							NormalizedValue<float> average_health;
					};

					struct Morphology {
							float                  volume_growth;
							NormalizedValue<float> average_volume;
					};

				} // namespace EmbryoGenomes

				namespace TransformationsInits {}

				struct CreatureGenome {
						Genetic::CreatureGenomes::Metabolism::MetabolismGenome     metabolism;
						Genetic::CreatureGenomes::Vital::VitalGenome               vital;
						Genetic::CreatureGenomes::Morphology::MorphologyGenome     morphology;
						Genetic::CreatureGenomes::Reproduction::ReproductiveGenome reproductive;
				};

				struct EmbryoGenome {
						Genetic::EmbryoGenomes::BodyGenome body;
						Genetic::EmbryoGenomes::Morphology morphology;
				};

				struct CoreGenome {
						Properties::PropertiesContainer properties;
						Creature::VisionRadius          vision_radius;
						Creature::Temperament           temperament;
						Creature::CreatureSpecies       specie;
				};

				struct BrainGenome {
						NormalizedValue<float>
							average_sociability; // create now, preparing for the future
				};

				struct Genome {
						CreatureGenome creature_genome;
						EmbryoGenome   embryo_genome;
						CoreGenome     core_genome;
						BrainGenome    brain_genome;

						[[nodiscard]] constexpr float calculate_embryo_average_max_energy() const {
							return embryo_genome.body.average_max_energy.value()
								* creature_genome.metabolism.max_energy;
						}

						[[nodiscard]] constexpr Volume calculate_embryo_average_volume() const {
							return embryo_genome.morphology.average_volume.value()
								* Size::volume(creature_genome.morphology.average_lateral,
											   creature_genome.morphology.average_height,
											   creature_genome.morphology.average_back)
									  .value;
						}

						[[nodiscard]] constexpr float calculate_embryo_average_life() const {
							return embryo_genome.body.average_max_life.value()
								* creature_genome.vital.max_life;
						}
				};

				class GenomeDatabase {
					public:

						std::flat_map<Creature::CreatureSpecies, EmbryoGenome>   g_embryos;
						std::flat_map<Creature::CreatureSpecies, CreatureGenome> g_creatures;
						std::flat_map<Creature::CreatureSpecies, CoreGenome>     g_core;
						std::flat_map<Creature::CreatureSpecies, BrainGenome>    g_brain;


						// flat_map is preferred here for the same reason array is preferred above:
						// the key set is small and stable, so contiguous storage is preferable to
						// hashing.

						bool   exists_specie(Creature::CreatureSpecies c_s) const;
						Genome get_genome(Creature::CreatureSpecies c_s) const;

						void add(Creature::CreatureSpecies c_s, CreatureGenome &&g);
						void add(Creature::CreatureSpecies c_s, EmbryoGenome &&g);
						void add(Creature::CreatureSpecies c_s, CoreGenome &&g);
						void add(Creature::CreatureSpecies c_s, BrainGenome &&g);
				};

				namespace Populate {
					void populate_crocodile(GenomeDatabase &creature_genomes);
					void populate_fish(GenomeDatabase &creature_genomes);
					void populate_crab(GenomeDatabase &creature_genomes);
					void populate_hippopotamus(GenomeDatabase &creature_genomes);
				} // namespace Populate

				inline GenomeDatabase init() {
					GenomeDatabase cg{};

					Populate::populate_crab(cg);
					Populate::populate_fish(cg);
					Populate::populate_crocodile(cg);
					Populate::populate_hippopotamus(cg);
					return cg;
				}

				inline GenomeDatabase global_creature_genomes = init();
			} // namespace Genetic

		} // namespace Entities
	} // namespace Organisms
} // namespace Karkinolution
