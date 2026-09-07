#pragma once

#include <karkinolution/math/physic/vec/model.hpp>
#include <karkinolution/organism/entities/creature/creature.hpp>
#include <karkinolution/organism/entities/creature/ontology.hpp>
#include <karkinolution/organism/entities/genetics/genetic.hpp>
#include <karkinolution/utils/k_random.hpp>

#include <cstdint>
#include <optional>
#include <string>

// All fields are optional: when absent they are randomly chosen or derived
// from the species canonical genome (global_creature_genomes).
struct CreatureBlueprint {
        std::optional<CreatureSpecies> specie   = std::nullopt; // random if absent
        std::optional<uint64_t>        id       = std::nullopt; // random if absent
        std::optional<std::string>     name     = std::nullopt; // random if absent
        std::optional<Gender>          gender   = std::nullopt; // random if absent
        std::optional<Vec3>            position = std::nullopt; // zero-vector if absent
};

namespace CreatureConstructor {

[[nodiscard]] Creature from_blueprint(const CreatureBlueprint &blueprint = {});

} // namespace CreatureConstructor
