#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <flat_map>
#include <stdexcept>
#include <utility>
#include <variant>
#include <vector>


#include <karkinolution/math/stats/compile_values.hpp>
#include <karkinolution/math/units.hpp>
#include <karkinolution/utils/k_random.hpp>

#include <karkinolution/organism/stats.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/properties/properties.hpp>
#include <karkinolution/organism/reproduction/state/ontology.hpp>

using OrganismStats::Life;
using OrganismStats::Health::Health;
using OrganismStats::Health::Immunity;
using OrganismStats::Muscle::Muscle;
using OrganismStats::Muscle::MuscleEfficiency;
using OrganismStats::Muscle::MuscleQuality;
using OrganismStats::Skeleton::Bone;
using OrganismStats::Skeleton::SkeletonQuality;
using OrganismStats::Time::Age;
using OrganismStats::Time::FertilityCooldown;
using OrganismStats::Time::GrowthRate;

using PhysicsStats::SharedVolume;
using PhysicsStats::Volume;
using PhysicsStats::Mass;

// ============================================================================
// COPYRIGHT & LICENSE
// ============================================================================

/*
 * Copyright 2026 Koöperative Rüpestrën
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 */

 
enum class CreatureSpecies : uint8_t {
    CRAB,
    FISH,
    CROCODILE,
    HIPPOPOTAMUS
};

namespace CreatureSpeciesF {

    CreatureSpecies choice();

}

 
namespace Genomes {

    enum class Resource : uint8_t {
        RESERVED_ENERGY
    };

    template <typename Trait>
    struct Relation {
        Resource resource;
        Trait target;
        Efficiency efficiency;
    };


    template <typename Trait>
    class Transformations {
    private:
        std::flat_map<Resource, std::vector<Relation<Trait>>> data;

    public:
        explicit Transformations(
            std::flat_map<Resource, std::vector<Relation<Trait>>>&& transformations
        )
            : data(std::move(transformations))
        {}

        Transformations()
            : data()
        {}

        void add(
            Resource resource,
            Relation<Trait> transformation
        ) {
            data[resource].push_back(std::move(transformation));
        }

        const std::vector<Relation<Trait>>& at(
            Resource resource
        ) const {
            return data.at(resource);
        }

        const Relation<Trait>& at(
            Resource resource,
            Trait trait
        ) const {
            const auto& transformations = data.at(resource);

            auto it = std::find_if(
                transformations.begin(),
                transformations.end(),
                [&](const Relation<Trait>& transformation) {
                    return transformation.target == trait;
                }
            );

            if (it == transformations.end()) {
                throw std::out_of_range("Transformation not found");
            }

            return *it;
        }

        const std::flat_map<
            Resource,
            std::vector<Relation<Trait>>
        >& view() const {
            return data;
        }
    };


 
    namespace CreatureGenomes {

        namespace Metabolism {

            enum class Trait : uint8_t {
                MAX_RESERVED_ENERGY,
                MAX_ENERGY,
                ENERGY
            };

            using Relation = Genomes::Relation<Trait>;
            using Transformations = Genomes::Transformations<Trait>;

            struct GrowthRates {
                GrowthRate max_energy;
                GrowthRate energy;
                GrowthRate max_reserved_energy;
            };

            struct MetabolismGenome {
                float max_energy;
                float max_reserved_energy;
                Diet diet;
                Transformations transformations;
                GrowthRates growth_rates;
            };

        } // namespace Metabolism


        // 3.2: Vital
        namespace Vital {

            enum class Trait : uint8_t {
                MAX_LIFE
            };

            using Relation = Genomes::Relation<Trait>;
            using Transformations = Genomes::Transformations<Trait>;

            struct GrowthRates {
                GrowthRate life;
            };

            struct VitalGenome {
                float max_life;
                Immunity average_immunity;
                Health average_health;
                Age average_age;
                Transformations transformations;
                GrowthRates growth_rates;
            };

        } 
        namespace MuscleStructure {

            enum class Trait : uint8_t {
                MUSCLE
            };

            using Relation = Genomes::Relation<Trait>;
            using Transformations = Genomes::Transformations<Trait>;

            struct GrowthRates {
                GrowthRate muscle;
            };

            struct MuscleGenome {
                SharedVolume shared_volume;
                Muscle average_muscles;
                MuscleEfficiency average_muscle_efficiency;
                MuscleQuality average_muscle_quality;
                GrowthRates growth_rates;
                Transformations transformations;
            };

        }
        namespace SkeletonStructure {

            enum class Trait : uint8_t {
                BONES
            };

            using Relation = Genomes::Relation<Trait>;
            using Transformations = Genomes::Transformations<Trait>;

            struct GrowthRates {
                GrowthRate bone;
            };

            struct SkeletonGenome {
                SharedVolume shared_volume;
                Bone average_bones;
                SkeletonQuality average_bone_quality;
                GrowthRate growth;
                Transformations transformations;
            };

        } 

        namespace Morphology {

            enum class Trait {
                HEIGHT,
                BACK,
                LATERAl
            };

            using Relation = Genomes::Relation<Trait>;
            using Transformations = Genomes::Transformations<Trait>;

            struct MorphologyGenome {
                Lateral average_lateral;
                Back average_back;
                Height average_height;
                Mass average_mass;
                Transformations transformations;
            };

        } 

        namespace Reproduction {

            struct ViviparousOrganismGenome {
                NormalizedValue<float> reproduction_cost;
                int fertility_limit;
                int gestation_limit;
                int average_children_count;
            };

            struct OviparousOrganismGenome {
            };
            
            struct ReproductiveGenome {
                std::variant<ViviparousOrganismGenome, OviparousOrganismGenome> reproductive_way_genome;
                int average_children_count;
                ReproductiveWays reproductive_way;
            };
        }

    } 


 
    namespace EmbryoGenomes {

        struct BodyGenome {
            NormalizedValue<float> average_max_energy;
            NormalizedValue<float> average_max_life;
            NormalizedValue<float> average_health;
        };

        struct Morphology {
            float volume_growth;
            NormalizedValue<float> average_volume;
        };

    } 

 
}

namespace TransformationsInits {
    
}

struct CreatureGenome {
    Genomes::CreatureGenomes::Metabolism::MetabolismGenome metabolism;
    Genomes::CreatureGenomes::Vital::VitalGenome vital;
    Genomes::CreatureGenomes::MuscleStructure::MuscleGenome muscle;
    Genomes::CreatureGenomes::SkeletonStructure::SkeletonGenome skeleton;
    Genomes::CreatureGenomes::Morphology::MorphologyGenome morphology;
    Genomes::CreatureGenomes::Reproduction::ReproductiveGenome reproductive;
};

struct EmbryoGenome {
    Genomes::EmbryoGenomes::BodyGenome body;
    Genomes::EmbryoGenomes::Morphology morphology;
};

 
struct CoreGenome {
    PropertiesContainer properties;
    VisionRadius vision_radius;
    Temperament temperament;
    CreatureSpecies specie;
};

 
struct BrainGenome {
    NormalizedValue<float> average_sociability; // create now, preparing for the future
};

 
struct Genome {
    CreatureGenome creature_genome;
    EmbryoGenome embryo_genome;
    CoreGenome core_genome;
    BrainGenome brain_genome;
};



 

 
class CreatureGenomes {
public:
    std::flat_map<CreatureSpecies, EmbryoGenome> g_embryos;
    std::flat_map<CreatureSpecies, CreatureGenome> g_creatures;
    std::flat_map<CreatureSpecies, CoreGenome> g_core;
    std::flat_map<CreatureSpecies, BrainGenome> g_brain;


    // flat_map is preferred here for the same reason array is preferred above:
    // the key set is small and stable, so contiguous storage is preferable to hashing.

    bool exists_specie(CreatureSpecies c_s) const;
    Genome get_genome(CreatureSpecies c_s) const;

    void add(CreatureSpecies c_s, CreatureGenome&& g);
    void add(CreatureSpecies c_s, EmbryoGenome&& g);
    void add(CreatureSpecies c_s, CoreGenome&& g);
    void add(CreatureSpecies c_s, BrainGenome&& g);
};

 
namespace Populate {
    void populate_crocodile(CreatureGenomes& creature_genomes);
    void populate_fish(CreatureGenomes& creature_genomes);
    void populate_crab(CreatureGenomes& creature_genomes);
    void populate_hippopotamus(CreatureGenomes& creature_genomes);
}

 
inline CreatureGenomes init() {
    CreatureGenomes cg{};

    Populate::populate_crab(cg);
    Populate::populate_fish(cg);
    Populate::populate_crocodile(cg);
    Populate::populate_hippopotamus(cg);
    return cg;
}

inline CreatureGenomes global_creature_genomes = init();