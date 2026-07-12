from dataclasses import dataclass
from karkinolution.decisions.presets import EatPreset, MovePreset, AttackPreset
from karkinolution.systems.metabolism import FoodOption, FoodHint, MetabolismSystem
from karkinolution.decisions.perception import Perception, PerceptionAnalyser, PerceivedCreature, PerceptionPatterns
from karkinolution.organism.creatures import Creature, EntitiesRegistry
from karkinolution.decisions.brain.instincts import AttackOutput, ScorerIntents, AttackResolver
from karkinolution.terrain.map import Territory
from karkinolution.systems.reproduction import ReproductiveSystem
from karkinolution.decisions.actions import IntentActs
from karkinolution.core.coord import Coord
from karkinolution.decisions.brain.factors import Factors
from random import uniform
from karkinolution.utils.k_random import choice_bool


NOISE_NOTHING_INTENT = 0.619

@dataclass(frozen=True)
class ScoredPreset:
    preset:EatPreset | MovePreset | AttackPreset
    score:float

class PlannerFindFood:
    @staticmethod
    def decide_preset(food_target:FoodOption, perception:Perception) -> MovePreset | EatPreset | AttackPreset:
        # ALIAS
        distance = perception.coord.distance_to_other(food_target.coord)
        food_type = food_target.food_hint
        block = perception.get(food_target.coord)
        # CODE
        
        if distance == 0 and food_type == FoodHint.GRASS:
            assert block.cell.food is not None
            return EatPreset(block.cell.food, food_type)

        if distance == 1:
            if food_type == FoodHint.CORPSE:
                assert isinstance(block.entity, PerceivedCreature)
                return EatPreset(block.entity.energy, food_type)
            elif food_type == FoodHint.TARGET:
                assert isinstance(block.entity, PerceivedCreature)
                return AttackPreset(block.entity.identity)
        return MovePreset(food_target.coord)
    @staticmethod
    def plan_intent(perception: Perception, creature: Creature, territory:Territory) -> MovePreset | EatPreset | AttackPreset | None:
        chosen = MetabolismSystem.choose_best(perception, creature, territory)
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
    @dataclass(frozen=True)
    class CoordScored:
        coord:Coord
        score:float
    @staticmethod
    def score_coord(perception:Perception, coord:Coord, creature:Creature) -> float:
        block = perception.get(coord)

        has_same_specie = block.has_creature and block.entity.specie_id == creature.genome.core.id
        has_creature = block.has_creature
        distance = creature.position.distance_to_other(coord)/perception.max_distance


        affinity = 1 if has_same_specie else (0.4 if has_creature else 0)
        factors = Factors.get_territorial_social_indifference_factor(creature) * Factors.get_pregnant_social_interest_factor(creature)
        
        return affinity * factors * creature.sociability.value - distance
    



    @staticmethod
    def plan_intent(perception:Perception, creature:Creature) -> MovePreset | None:
        chosen = choice_bool(yes_weight=1-creature.hungry, no_weight=creature.hungry)

        if not chosen:
            return None
        
        coords_s:list[PlannerNothing.CoordScored] = []
        
        new_perception = PerceptionAnalyser.get_area_in_radius_ratio(perception, 0.7)
        for c in new_perception.coords:
                coords_s.append(
                    PlannerNothing.CoordScored(
                        c,
                        PlannerNothing.score_coord(perception, c, creature) *  uniform(1, 1+NOISE_NOTHING_INTENT)
                    )
                )

        return MovePreset(max(coords_s, key=lambda x: x.score).coord)
    
        
        

@dataclass(frozen=True)
class PlannerScored:
    score:float

class Planner:
    @staticmethod
    def resolve_intent(perception:Perception, creature:Creature, territory:Territory) -> MovePreset | EatPreset | AttackPreset | None:
        intent = creature.intent.intent
        if intent == IntentActs.FIND_FOOD:
            return PlannerFindFood.plan_intent(perception, creature, territory)
        elif intent == IntentActs.FIND_MATCH:
            return PlannerFindMatch.plan_intent(perception, creature)
        elif intent == IntentActs.NOTHING:
            return PlannerNothing.plan_intent(perception, creature)
        return None
    @staticmethod
    def plan(perception:Perception, creature:Creature, entities:EntitiesRegistry, territory:Territory) -> MovePreset | EatPreset | AttackPreset | None:
        result_attack = AttackResolver.resolve_attack(creature, perception, entities)
        


        
        if result_attack is None:
            return Planner.resolve_intent(perception, creature, territory)
        


        if creature.intent.intent == IntentActs.FIND_FOOD:
            planner_scored = PlannerScored(ScorerIntents.score_find_food(creature))
        elif creature.intent.intent == IntentActs.FIND_MATCH:
            planner_scored = PlannerScored(ScorerIntents.score_find_match(creature))
        else: # NOTHING INTENT
            planner_scored = PlannerScored(ScorerIntents.score_nothing(creature))
        
        if result_attack.score > planner_scored.score:
            return PlannerAttack.plan_intent(result_attack, perception, entities)
        return Planner.resolve_intent(perception, creature, territory)