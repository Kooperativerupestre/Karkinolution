from dataclasses import dataclass
from karkinolution.decisions.presets import EatPreset, MovePreset, AttackPreset
from karkinolution.systems.metabolism import FoodOption, FoodHint, MetabolismSystem
from karkinolution.decisions.perception import Perception, PerceptionAnalyser, PerceivedCreature, PerceptionPatterns, DangerIndex, PerceivedCorpse
from karkinolution.organism.creatures import Creature, EntitiesRegistry
from karkinolution.decisions.brain.instincts import AttackOutput, ScorerIntents, AttackResolver
from karkinolution.terrain.map import Territory
from karkinolution.systems.reproduction import ReproductiveSystem
from karkinolution.decisions.actions import IntentActs
from karkinolution.core.coord import Coord
from karkinolution.decisions.brain.factors import Factors
from random import uniform
from karkinolution.utils.k_random import choice_bool
from karkinolution.systems.physics import MovementSystem, AttackSystem
from karkinolution.organism.stats import LimitedValue

NOISY_NOTHING_INTENT = 0.619
NOISY_HITS_TO_KILL = 0.1
MIN_DIET_PREFERENCE_EFFECTIVENESS = 0.25

@dataclass(frozen=True)
class ScoredPreset:
    preset:EatPreset | MovePreset | AttackPreset
    score:float
@dataclass(frozen=True)
class CoordScored:
    coord:Coord
    score:float

@dataclass(frozen=True)
class FoodCoord:
    coord:Coord
    food_hint:FoodHint
    score:float
    distance:float
        
class PlannerFindFood:
    @staticmethod
    def can_eat_food_option(food_option:FoodOption, creature:Creature) -> bool:
        diet_bias = creature.genome.metabolism.diet[food_option.food_hint]
        diet_effective = MetabolismSystem.diet_effective(diet_bias, creature.hungry)
        return diet_effective >= MIN_DIET_PREFERENCE_EFFECTIVENESS


    @staticmethod
    def score_food_option(food_option:FoodOption, creature:Creature, danger_index:DangerIndex, perception:Perception) -> float:
        movement_cost = MovementSystem.calculate_cost_distance_to_move(food_option.distance, creature)
        cost = movement_cost**1.2
        diet_bias = creature.genome.metabolism.diet[food_option.food_hint]

        if food_option.food_hint == FoodHint.TARGET:
            assert isinstance(perception.get(food_option.coord).entity, PerceivedCreature)
            cost += AttackSystem.calculate_cost_to_kill(creature, perception.get(food_option.coord).entity) * uniform(1 - NOISY_HITS_TO_KILL, 1 + NOISY_HITS_TO_KILL)
        
        normalized_cost = LimitedValue(0, 1)
        normalized_gain = LimitedValue(0, 1)
        
        normalized_cost.set(cost/creature.energy.limit)
        normalized_gain.set(food_option.energy_gain/creature.energy.limit)

        food_urge = MetabolismSystem.diet_effective(diet_bias, creature.hungry) * normalized_gain.value

        value = LimitedValue(food_urge - normalized_cost.value, 1, -1)

        danger = danger_index.try_get(food_option.coord)
        if danger is None:
            value.sub(0.1)
        else:
            value.sub(danger.accumulated_danger)

        return value.value
    
    @staticmethod
    def chose_best_food_coord(perception:Perception, danger_index:DangerIndex, creature:Creature) -> FoodCoord:
        options:list[FoodCoord] = []

        for c in perception.coords:
            food_options:list[FoodOption] = MetabolismSystem.get_food_options(perception, c) # max options: 3

            for f_o in food_options:
                options.append(FoodCoord(
                    c,
                    f_o.food_hint,
                    PlannerFindFood.score_food_option(f_o, creature, danger_index, perception),
                    f_o.distance
                ))
        return max(options, key=lambda x: x.score)
    @staticmethod
    def decide_preset(food_coord:FoodCoord, perception:Perception) -> MovePreset | EatPreset | AttackPreset:
        block = perception.get(food_coord.coord)
        if food_coord.distance == 0 and food_coord.food_hint == FoodHint.GRASS:
            assert block.cell.food is not None
            return EatPreset(block.cell.food, food_coord.food_hint)
        if food_coord.distance == 1:
            if food_coord.food_hint == FoodHint.CORPSE:
                assert isinstance(block.entity, PerceivedCorpse)
                return EatPreset(block.entity.energy, FoodHint.CORPSE)
            elif food_coord.food_hint == FoodHint.TARGET:
                assert isinstance(block.entity, PerceivedCreature)
                return AttackPreset(block.entity.id)
        return MovePreset(food_coord.coord)
        
    @staticmethod
    def plan_intent(perception:Perception, danger_index:DangerIndex, creature:Creature) -> MovePreset | EatPreset | AttackPreset | None:
        chosen = PlannerFindFood.chose_best_food_coord(perception, danger_index, creature)
        if chosen is None:
            return None
        return PlannerFindFood.decide_preset(chosen, perception)

class PlannerFindMatch:
    @staticmethod
    def plan_intent(perception:Perception, creature:Creature) -> MovePreset | None:
        possibly_matches = list(PerceptionPatterns.same_species(perception, predicate=lambda b, _: ReproductiveSystem.can_reproduce(creature, b.entity)).coords)
        if len(possibly_matches) == 0:
            return None
        
        near_coord = PerceptionAnalyser.near_coord(creature.position, possibly_matches)

        if perception.coord.distance_exceeds_one(near_coord):
            return MovePreset(near_coord)

class PlannerAttack:
    @staticmethod
    def plan_intent(attack_output:AttackOutput, perception:Perception, entities:EntitiesRegistry) -> MovePreset | AttackPreset:
        target_coord = entities.get_creature(attack_output.target_id).position

        if perception.coord.distance_to_other(target_coord) > 1:
            return MovePreset(target_coord)
        return AttackPreset(attack_output.target_id)

class PlannerNothing:
    @staticmethod
    def score_coord(perception:Perception, coord:Coord, creature:Creature) -> float:
        block = perception.get(coord)

        has_creature = block.has_creature
        has_same_specie = has_creature and perception.get(coord).entity.ontology.specie == creature.genome.core.specie
        distance = creature.position.distance_to_other(coord)/perception.max_distance


        affinity = 1 if has_same_specie else (0.4 if has_creature else 0)
        factors = Factors.get_territorial_social_indifference_factor(creature) * Factors.get_pregnant_social_interest_factor(creature)
        
        return affinity * factors * creature.sociability.value - distance


    @staticmethod
    def plan_intent(perception:Perception, creature:Creature) -> MovePreset | None:
        chosen = choice_bool(yes_weight=1-creature.hungry, no_weight=creature.hungry)

        if not chosen:
            return None
        
        coords_s:list[CoordScored] = []
        
        new_perception = PerceptionAnalyser.get_area_in_radius_ratio(perception, 0.7)
        for c in new_perception.coords:
                coords_s.append(
                    CoordScored(
                        c,
                        PlannerNothing.score_coord(perception, c, creature) *  uniform(1, 1+NOISY_NOTHING_INTENT)
                    )
                )

        return MovePreset(max(coords_s, key=lambda x: x.score).coord)
    
        
        

@dataclass(frozen=True)
class PlannerScored:
    score:float

class Planner:
    @staticmethod
    def resolve_intent(perception:Perception, danger_index:DangerIndex, creature:Creature) -> MovePreset | EatPreset | AttackPreset | None:
        intent = creature.intent.intent
        if intent == IntentActs.FIND_FOOD:
            return PlannerFindFood.plan_intent(perception, danger_index, creature)
        elif intent == IntentActs.FIND_MATCH:
            return PlannerFindMatch.plan_intent(perception, creature)
        elif intent == IntentActs.NOTHING:
            return PlannerNothing.plan_intent(perception, creature)
        return None
    @staticmethod
    def plan(perception:Perception, danger_index:DangerIndex, creature:Creature, entities:EntitiesRegistry) -> MovePreset | EatPreset | AttackPreset | None:
        result_attack = AttackResolver.resolve_attack(creature, perception, entities)
        


        
        if result_attack is None:
            return Planner.resolve_intent(perception, danger_index, creature)
        


        if creature.intent.intent == IntentActs.FIND_FOOD:
            planner_scored = PlannerScored(ScorerIntents.score_find_food(creature))
        elif creature.intent.intent == IntentActs.FIND_MATCH:
            planner_scored = PlannerScored(ScorerIntents.score_find_match(creature))
        else: # NOTHING INTENT
            planner_scored = PlannerScored(ScorerIntents.score_nothing(creature))
        
        if result_attack.score > planner_scored.score:
            return PlannerAttack.plan_intent(result_attack, perception, entities)
        return Planner.resolve_intent(perception, danger_index, creature)