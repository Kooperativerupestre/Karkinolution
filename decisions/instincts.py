from organism.ontology import Temperament
from systems.metabolism_system import FoodOption, FoodHint, MetabolismSystem
from decisions.perception import Perception, Analysis, PerceivedCreature, PerceivedBlock
from organism.creatures import Creature, PregnantUterus, EntitiesRegistry
from decisions.actions import Intent, IntentActs
from decisions.presets import MovePreset, EatPreset, AttackPreset
from organism.stats import LimitedValue
from systems.reproductivebuffer import ReproductiveBuffer, ReproductiveDesire
from dataclasses import dataclass
from core.error import EntityError
from random import uniform
from organism.identity import EntityTypes
from systems.reproduction import ReproductiveSystem
from organism.identity import Id
from core.coord import Coord
from typing import Callable
from random import choices

COURAGE_FACTOR:dict[Temperament, float] = {
    Temperament.PASSIVE: 0.1,
    Temperament.NEUTRAL: 0.3,
    Temperament.AGGRESSIVE: 0.7,
    Temperament.TERRITORIAL: 1
}



TRADE_OFF:dict[Temperament, float] = {
    Temperament.PASSIVE: 0.1,
    Temperament.NEUTRAL: 0.5,
    Temperament.AGGRESSIVE: 0.89,
    Temperament.TERRITORIAL: 1
}

@dataclass(frozen=True)
class Config:
    noise:float


GLOBAL_CONFIG = Config(0.1)

class FactorsCalc:
    '''
    1 -> All outputs must be in the interval [0, 1]
    2 -> All factors here must accept a creature as their only argument
    3 -> All antonyms cannot always be computed using a simple 1 - constant formula; most will require their own specific formulas.
    '''
    @staticmethod
    def get_pregnancy_risk(creature:Creature) -> float:
        factor = LimitedValue(0, 1)

        if not creature.pregnant:
            raise EntityError('Only pregnant creatures have pregnancy risk constant')
        assert isinstance(creature.uterus, PregnantUterus)

        factor.add(creature.uterus.gravity * 0.75)
        factor.sub(creature.uterus.number_children.ratio * 0.35)

        return factor.value
    @staticmethod
    def get_fear(creature:Creature) -> float:
        factor = LimitedValue(0, 1)

        ph_r = creature.physical_ratio

        if ph_r < 0.30:
            factor.add(ph_r - 0.05)

        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            factor.add(creature.uterus.gravity)
        
        factor.add(creature.senescence/1.7)
        return factor.value
    @staticmethod
    def get_courage(creature:Creature) -> float:
        basal = LimitedValue(COURAGE_FACTOR[creature.genome.core.behavior], 1)

        basal.sub(creature.senescence)

        basal.add(creature.genome.metabolism.mass/3)

        return basal.value
    
@dataclass(frozen=True)
class AttackOutput:
    score:float
    target_id:Id


class AttackResolver:
    @staticmethod
    def score_attack(creature:Creature, target:Creature) -> float:
        return (creature.physical_ratio - target.physical_ratio + 1)/2 # [0, 1]

    @staticmethod
    def get_reactive_attack(creature:Creature, entities:EntitiesRegistry) -> AttackOutput | None:
        if creature.last_attack is not None:
            target_id = creature.last_attack.attacker_id
            score = AttackResolver.score_attack(creature, entities.get_creature(target_id))

            return AttackOutput(
                score,
                target_id
            )
    @staticmethod
    def resolve_predicate_attack(creature:Creature,
                                radius:Coord,
                                perception:Perception,
                                predicate:Callable[[PerceivedBlock], bool]) -> Coord | None:
        area = perception.neighbors_x_y(radius)
        targets_coords:list[Coord] = []

        for c, b in area:
            if b.get_entity_type() == EntityTypes.CREATURE and predicate(b):
                targets_coords.append(c)
        if len(targets_coords) == 0:
            return None
        target_coord = Analysis.near_coord(targets_coords, creature.position)
        return target_coord
    
    @staticmethod
    def resolve_aggressive_attack(creature:Creature, perception:Perception, entities:EntitiesRegistry) -> AttackOutput | None:
        target_coord = AttackResolver.resolve_predicate_attack(creature, Coord(3, 2), perception, lambda b: b.entity.specie_id == creature.genome.core.id) # type: ignore
        if target_coord is None:
            return None
        target_id = perception.get(target_coord).entity.identity # type: ignore
        score = AttackResolver.score_attack(creature, entities.get_creature(target_id))
        return AttackOutput(
            score,
            target_id
        )
    @staticmethod
    def resolve_territorial_attack(creature:Creature, perception:Perception, entities:EntitiesRegistry) -> AttackOutput | None:
        target_coord = AttackResolver.resolve_predicate_attack(creature, Coord(2, 2), perception, lambda b: True)
        if target_coord is None:
            return None
        target_id = perception.get(target_coord).entity.identity # type: ignore
        score = choices([0, 1], weights=[1/2, 2/3], k=1)[0]
        return AttackOutput(
            score,
            target_id
        )

    @staticmethod
    def resolve_attack(creature:Creature, perception:Perception, entities:EntitiesRegistry) -> AttackOutput | None:
        # ALIAS
        temperament = creature.genome.core.behavior
        # CODE
        attack_output = AttackResolver.get_reactive_attack(creature, entities)

        if attack_output is None:
            if temperament == Temperament.AGGRESSIVE:
                attack_output = AttackResolver.resolve_aggressive_attack(creature, perception, entities)
            elif temperament == Temperament.TERRITORIAL:
                attack_output = AttackResolver.resolve_territorial_attack(creature, perception, entities)
        
        return attack_output
    

class ScorerIntents:
    @staticmethod
    def score_find_food(creature:Creature) -> float:
        factor = LimitedValue(0, 1)

        hungry = creature.hungry

        if hungry > creature.genome.metabolism.max_hungry:
            factor.add(hungry * 1.3)
        else:
            factor.add(hungry * 1.12)
        
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            factor.add(creature.uterus.gravity/9)
        return factor.value
    
    @staticmethod
    def score_find_match(creature:Creature) -> float:
        factor = LimitedValue(0, 1)

        factor.add(creature.reproductive_maturity*0.9)
        
        factor.mul(creature.reproductive_fitness)

        return factor.value
    @staticmethod
    def score_nothing(creature:Creature) -> float:
        base = 0.6

        factor = LimitedValue(base, 1)


        factor.add(creature.senescence/4)
        return factor.value
    
@dataclass
class IntentScored:
    act:IntentActs
    score:float


    def mul(self, value:float | int) -> None:
        self.score *= value

class Instincts:
    @staticmethod
    def take(creature:Creature, reproductive_buffer:ReproductiveBuffer) -> list[IntentScored]:
        acts:list[IntentScored] = []
        # FIND_FOOD
        acts.append(IntentScored(
            IntentActs.FIND_FOOD,
            ScorerIntents.score_find_food(creature)
        ))
        # NOTHING
        acts.append(IntentScored(
            IntentActs.NOTHING,
            ScorerIntents.score_nothing(creature)
        ))
        # FIND MATCH
        if creature.reproductively_capable and reproductive_buffer.require(creature.id) is None:
            acts.append(IntentScored(
                IntentActs.FIND_MATCH,
                ScorerIntents.score_find_match(creature)
            ))
        return acts
    @staticmethod
    def apply_noise(acts:list[IntentScored]) -> None:
        for act in acts:
            act.mul(uniform(1, 1+GLOBAL_CONFIG.noise))
    
    @staticmethod
    def chose(acts:list[IntentScored]) -> Intent:
        chosen = max(acts, key=lambda a: a.score)


        return Intent(chosen.act)


def try_call_reproductive_buffer(intent:Intent, creature:Creature, reproductive_buffer:ReproductiveBuffer) -> None:
    if intent.intent == IntentActs.FIND_MATCH:
        reproductive_buffer.registry(ReproductiveDesire(creature.id, creature.genome.core.id))
    


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
    def plan_intent(perception: Perception, creature: Creature) -> MovePreset | EatPreset | AttackPreset | None:
        chosen = MetabolismSystem.choose_best(perception, creature)
        if chosen is None:
            return None
        
        return PlannerFindFood.decide_preset(chosen, perception)
    

class PlannerFindMatch:
    @staticmethod
    def plan_intent(perception:Perception, creature:Creature) -> MovePreset | None:
        same_specie = Analysis.find_predicate(perception, predicate=
                                              lambda b: b.get_entity_type() == EntityTypes.CREATURE and
                                                ReproductiveSystem.can_reproduce(creature, b.entity)) # type: ignore
        if len(same_specie) == 0:
            return None
        
        near_coord = Analysis.near_coord(same_specie, perception.coord)

        if perception.coord.distance_exceeds_one(near_coord):
            return MovePreset(near_coord)

class PlannerAttack:
    @staticmethod
    def plan_intent(attack_output:AttackOutput, perception:Perception, entities:EntitiesRegistry) -> MovePreset | AttackPreset:
        target_coord = entities.get_creature(attack_output.target_id).position

        if perception.coord.distance_to_other(target_coord) > 1:
            return MovePreset(target_coord)
        return AttackPreset(attack_output.target_id)

@dataclass(frozen=True)
class PlannerScored:
    score:float

class Planner:
    @staticmethod
    def resolve_intent(perception:Perception, creature:Creature) -> MovePreset | EatPreset | AttackPreset | None:
        intent = creature.intent.intent
        if intent == IntentActs.FIND_FOOD:
            return PlannerFindFood.plan_intent(perception, creature)
        elif intent == IntentActs.FIND_MATCH:
            return PlannerFindMatch.plan_intent(perception, creature)
        return None
    @staticmethod
    def plan(perception:Perception, creature:Creature, entities:EntitiesRegistry) -> MovePreset | EatPreset | AttackPreset | None:
        result_attack = AttackResolver.resolve_attack(creature, perception, entities)
        


        
        if result_attack is None:
            return Planner.resolve_intent(perception, creature)
        


        if creature.intent.intent == IntentActs.FIND_FOOD:
            planner_scored = PlannerScored(ScorerIntents.score_find_food(creature))
        elif creature.intent.intent == IntentActs.FIND_MATCH:
            planner_scored = PlannerScored(ScorerIntents.score_find_match(creature))
        else: # NOTHING INTENT
            planner_scored = PlannerScored(ScorerIntents.score_nothing(creature))
        
        if result_attack.score > planner_scored.score:
            return PlannerAttack.plan_intent(result_attack, perception, entities)
        return Planner.resolve_intent(perception, creature)
    