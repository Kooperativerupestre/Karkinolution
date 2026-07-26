#include <karkinolution/terrain/map.hpp>
#include <karkinolution/terrain/soil.hpp>
#include <karkinolution/core/vec2.hpp>
#include <karkinolution/brain/perception.hpp>
#include <karkinolution/organism/ontology.hpp>
#include <optional>
#include <ranges>
#include <karkinolution/core/stats.hpp>
#include <variant>


bool PerceivedBlock::has_entity() const {
    return !std::holds_alternative<std::monostate>(entity);
}

std::optional<EntityTypes> PerceivedBlock::get_entity_type() const {
    if (std::holds_alternative<PerceivedCreature>(entity)) {
        return EntityTypes::CREATURE;
    }
    if (std::holds_alternative<PerceivedCorpse>(entity)) {
        return EntityTypes::CORPSE;  
    }
    return std::nullopt;
}

bool PerceivedBlock::has_creature() const {
    return std::holds_alternative<PerceivedCreature>(entity);
}

bool PerceivedBlock::has_corpse() const {
    return std::holds_alternative<PerceivedCorpse>(entity);
}

const PerceivedCreature& PerceivedBlock::get_creature() const {
    return std::get<PerceivedCreature>(entity);
}

const PerceivedCorpse& PerceivedBlock::get_corpse() const {
    return std::get<PerceivedCorpse>(entity);
}

// BlockPropeties

bool BlockProperties::has_aggressive_creature(const PerceivedBlock& block) {
    if (auto* creature = std::get_if<PerceivedCreature>(&block.entity)) {
        return creature->ontology.temperament == Temperament::AGGRESSIVE;
    }
    return false;
}

bool BlockProperties::has_territorial_creature(const PerceivedBlock& block) {
    if (auto* creature = std::get_if<PerceivedCreature>(&block.entity)) {
        return creature->ontology.temperament == Temperament::TERRITORIAL;
    }
    return false;
}

bool BlockProperties::is_same_specie(const PerceivedCreature& other, const ObserverCreature& creature) {
    return other.ontology.specie == creature.specie;
}

bool BlockProperties::is_more_strong(const PerceivedCreature& other, const ObserverCreature& creature) {
    return other.body.physical_ratio > creature.life_ratio;
}


// Perception

const_iterator Perception::begin() const {
    return pieces.begin();
}

const_iterator Perception::end() const {
    return pieces.end();
}

int Perception::size() const {
    return static_cast<int>(pieces.size());
}

const std::unordered_map<Vec2, PerceivedBlock>& Perception::pieces_ref() const {
    return pieces;
}

bool Perception::contains(Vec2 coord) const {
    return pieces.contains(coord);
}

bool Perception::empty() const {
    return pieces.empty();
}

const PerceivedBlock& Perception::at(Vec2 coord) const {
    return pieces.at(coord);
}

const PerceivedBlock* Perception::try_at(Vec2 coord) const {
    auto it = pieces.find(coord);
    if (it != pieces.end()) {
        return &(it->second); 
    }
    return nullptr;
}


const PerceivedBlock& Perception::creature_block() const {
    return pieces.at(coord());
}

const ObserverCreature& Perception::creature() const {
    return _creature;
}

Vec2 Perception::coord() const {
    return _coord;
}

float Perception::max_distance() const {
    return _max_distance;
}

// PerceptionAnalyser
Perception PerceptionAnalyser::build_another_perception(
    const Perception& old_perception,
    const std::unordered_map<Vec2, PerceivedBlock>& new_pieces) 
{
    Perception new_p{
        new_pieces,
        old_perception.creature(),
        old_perception.coord(),
        old_perception.max_distance()
    };
    return new_p;
}
    

Perception PerceptionAnalyser::neighbors_4(const Perception& perception, bool include_self) {
    return PerceptionAnalyser::build(perception, Vec2F::four_movements(perception.coord()));
}

Perception PerceptionAnalyser::neighbors_8(const Perception& perception, bool include_self) {
    return PerceptionAnalyser::build(perception, Vec2F::eight_movements(perception.coord()));
}


Perception PerceptionAnalyser::get_area_in_radius_ratio(const Perception& perception, float radius_ratio) {
    std::unordered_map<Vec2, PerceivedBlock> filtered;
    float current_max_search = perception.max_distance() * radius_ratio;

    for (const auto& [pos, block] : perception) {
        if (block.distance <= current_max_search) {
            filtered[pos] = block;
        }
    }
    return build_another_perception(perception, filtered);
}

Vec2 PerceptionAnalyser::near_coord(const Perception&perception) {
    auto size = perception.size();
    Vec2 near_coord = perception.begin()->first;
    float near_distance = perception.begin()->second.distance;
    
        for (auto i = 0; i < size + 1; i++) {
            const PerceivedBlock* block = &(perception.begin()++)->second;
            const float distance = block->distance;
            if (distance < near_distance) {
                near_coord = (perception.begin()++)->first;
                near_distance = distance;
            }
        }
    return near_coord;
}


// Dangers, DangerIndex, DangerFactory

float Dangers::accumulated_danger() const {
    return cell_danger.value() + creature_danger.value();
}

const Dangers& DangerIndex::at(Vec2 coord) const {
    return index.at(coord);
}

const Dangers* DangerIndex::try_at(Vec2 coord) const {
    auto it = index.find(coord);
    if (it != index.end()) {
        return &(it->second);
    }
    return nullptr;
}

void DangerIndex::add(Vec2 coord, const Dangers& dangers) {
    if (try_at(coord) == nullptr) {
        index[coord] = dangers;
    } 
}

// Perceiver

PerceivedCreature Perceiver::perceive_creature(const Creature&creature) {
    PerceivedBody body{
        .energy = creature.energy,
        .life = creature.life.value(),
        .physical_ratio = creature.physical_ratio(),
        .reproductive_capacity = creature.reproductively_capable()
    };

    PerceivedOntology ontology{
        .specie = creature.genome.core.specie,
        .temperament = creature.genome.core.temperament,
        .gender = creature.gender
    };

    return PerceivedCreature{
        .body = body,
        .ontology = ontology,
        .id = IDF::create_creature_id(creature.id)
    };
}

PerceivedCorpse Perceiver::perceive_corpse(const Corpse &corpse) {
    return PerceivedCorpse{
        .energy = corpse.energy,
        .id = IDF::create_corpse_id(corpse.id)
    };
}

PerceivedSoil Perceiver::perceive_soil(const SoilPiece &soil_piece) {
    bool is_movable = false;
    bool is_edible = false;
    bool is_dangerous = false;

    for (auto& property : soil_piece.properties) {
        if (property == Properties::DANGEROUS) {
            is_dangerous = true;
        } else if (property == Properties::EDIBLE) {
            is_edible = true;
        }
    }

    if (soil_piece.components.exists<MovementCost>()) { is_movable = true;}

    std::optional<float> movement_cost = std::nullopt, damage = std::nullopt;
    std::optional<Energy> food = std::nullopt;

    if (is_dangerous) {
        damage = soil_piece.components.try_get<Damage>()->damage;
    }

    if (is_edible) {
        food = soil_piece.components.try_get<FoodState>()->food;
    }
    if (is_movable) {
        movement_cost = soil_piece.components.try_get<MovementCost>()->cost;
    }
    return PerceivedSoil{
        .is_movable = is_movable,
        .is_edible = is_edible,
        .is_dangerous = is_dangerous,
        .movement_cost = movement_cost,
        .damage = damage,
        .food = food
    };
}


Perception Perceiver::perceive(const Creature &creature, const Territory &territory, const EntityMap &entity_map, const EntitiesRegistry &entities) {
    const Vec2 vision_radius = creature.genome.core.vision_radius;

    ObserverCreature observer_creature{
        .energy_ratio = creature.energy.ratio(),
        .life_ratio = creature.life.ratio(),
        .specie = creature.genome.core.specie,
        .id = IDF::create_creature_id(creature.id),
    };

    std::unordered_map<Vec2, PerceivedBlock> perceived{};
    perceived.reserve((2*creature.genome.core.vision_radius.x + 1) * (2*creature.genome.core.vision_radius.y + 1));

    for (int x = -creature.genome.core.vision_radius.x; x <= creature.genome.core.vision_radius.x; x++) {
        for (int y = -creature.genome.core.vision_radius.y; y <= creature.genome.core.vision_radius.y; y++) {
            const Vec2 coord{x + creature.position.x, y + creature.position.y};
            if (territory.exists(coord)) {
                PerceivedSoil perceived_soil = Perceiver::perceive_soil(territory.at(coord));
                
                const Id* id = entity_map.try_at(coord);
                const Entity* entity = nullptr;
                PerceivedEntity perceived_entity = std::monostate();
                if (id) {
                    entity = &entities.at(*id);
                    if (id->entity_type == EntityTypes::CREATURE) {
                        perceived_entity = PerceivedCreature{Perceiver::perceive_creature(std::get<Creature>(*entity))};
                    } else if (id->entity_type == EntityTypes::CORPSE) {
                        perceived_entity = PerceivedCorpse{Perceiver::perceive_corpse(std::get<Corpse>(*entity))};
                    }
                }
                perceived.emplace(coord,
                    PerceivedBlock{.cell=perceived_soil, .entity=perceived_entity, .distance=Vec2F::distance(creature.position, coord)});
            }
        }
    }
    
    return Perception{
    perceived,
    observer_creature,
    creature.position,
    creature.position.length()
    };
}


// DangerFactory

SignedNormalizedValue DangerFactory::get_creature_danger(const PerceivedBlock& block, const Creature& creature) {
    SignedNormalizedValue score{0};

    if (auto* other = std::get_if<PerceivedCreature>(&block.entity)) {
        if (BlockProperties::has_aggressive_creature(block)) {
            score += AGGRESSIVE_DANGER;
        }
        if (BlockProperties::has_territorial_creature(block)) {
            score += TERRITORIAL_DANGER;
        }
        score -= (creature.physical_ratio().value() - other->body.physical_ratio.value())/2;
    }

    return score.value();
}

SignedNormalizedValue DangerFactory::get_cell_danger(const PerceivedBlock& block) {
    SignedNormalizedValue danger = 0.0f;
    if (block.cell.is_dangerous) {
        danger += DANGEROUS_CELL_FACTOR;
    }
    if (block.cell.damage.has_value()) {
        danger += block.cell.damage.value() * 0.01f;
    }
    return danger;
}

Dangers DangerFactory::get_dangers(const PerceivedBlock& block, const Creature& creature) {
    return Dangers {
        .cell_danger = get_cell_danger(block),
        .creature_danger = get_creature_danger(block, creature)
    };
}

DangerIndex DangerFactory::create_danger_index(const Perception& perception, const Creature& creature) {
    DangerIndex danger_index;
    auto new_perception = PerceptionAnalyser::get_area_in_radius_ratio(perception, GET_DANGER_INDEX_AREA_RATIO);


    for (const auto& [coord, block] : perception) {
        Dangers block_dangers = get_dangers(block, creature);
        Dangers four_coords_dangers = Dangers{0, 0};
        std::vector<Vec2> four_movements = Vec2F::four_movements(coord);
        int count = 0; 
        
        for (auto f_c : four_movements)
            if (new_perception.contains(f_c)) {
                four_coords_dangers.cell_danger += DangerFactory::get_cell_danger(block).value();
                four_coords_dangers.creature_danger += DangerFactory::get_creature_danger(block, creature).value();
                count ++;
            }
        if (count > 0) {
            four_coords_dangers.cell_danger/= static_cast<float>(count);
            four_coords_dangers.creature_danger/= static_cast<float>(count);

            four_coords_dangers.cell_danger *= ADJACENT_DANGER_WEIGHT;
            four_coords_dangers.creature_danger *= ADJACENT_DANGER_WEIGHT;
        }

        Dangers main_dangers = {
            .cell_danger = block_dangers.cell_danger.value() + four_coords_dangers.cell_danger.value(),
            .creature_danger = block_dangers.creature_danger.value() + four_coords_dangers.creature_danger.value() 
        };
    }
    return danger_index;
}