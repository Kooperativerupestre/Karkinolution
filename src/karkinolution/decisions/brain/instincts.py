from dataclasses import dataclass
from karkinolution.organism.creatures import Creature, EntitiesRegistry, PregnantUterus
from karkinolution.core.coord import Coord
from karkinolution.decisions.perception import Perception, PerceptionAnalyser, PerceivedBlock, PerceptionPatterns
from karkinolution.decisions.brain.factors import Factors
from typing import Callable
from karkinolution.organism.identity import Id
from karkinolution.organism.ontology import Temperament
from karkinolution.organism.stats import LimitedValue
from random import choice, uniform
from karkinolution.decisions.actions import Intent, IntentActs
from karkinolution.systems.reproductivebuffer import ReproductiveBuffer, ReproductiveDesire


NOISE_INSTINCTS = 0.1


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
        if creature.last_attack is not None and creature.last_attack.attacker_id in entities.entitys:
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
                                predicate:Callable[[PerceivedBlock, Coord], bool] = lambda b, c: True) -> Coord | None:
        area = PerceptionAnalyser.neighbors_x_y(perception, radius.x, radius.y)

        coords = list(PerceptionPatterns.creatures(area, predicate).coords)

        if len(coords) == 0:
            return None
        target_coord = PerceptionAnalyser.near_coord(creature.position, coords)
        return target_coord

    
    @staticmethod
    def resolve_aggressive_attack(creature:Creature, perception:Perception, entities:EntitiesRegistry) -> AttackOutput | None:
        target_coord = AttackResolver.resolve_predicate_attack(creature, Coord(3, 2), perception, predicate=lambda b, _: b.entity.specie_id != perception.creature.specie_id)
        if target_coord is not None:
            target_id = perception.get(target_coord).entity.identity
            score = AttackResolver.score_attack(creature, entities.get_creature(target_id))
            return AttackOutput(
                score,
                target_id
            )
        
    
    @staticmethod
    def resolve_territorial_attack(creature:Creature, perception:Perception, entities:EntitiesRegistry) -> AttackOutput | None:
        target_coord = AttackResolver.resolve_predicate_attack(creature, Coord(2, 2), perception, predicate=lambda b, _: b.entity.specie_id != perception.creature.specie_id)
        if target_coord is None:
            return None
        target_id = perception.get(target_coord).entity.identity # type: ignore
        score = LimitedValue(choice([1/2, 2/3]), 1)

        if creature.pregnant:
            score.sub(Factors.get_pregnant_territorial_avoidance_attack_factor(creature))
        return AttackOutput(
            score.value,
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
            act.mul(uniform(1, 1+NOISE_INSTINCTS))
    
    @staticmethod
    def chose(acts:list[IntentScored]) -> Intent:
        chosen = max(acts, key=lambda a: a.score)


        return Intent(chosen.act)

    


def try_call_reproductive_buffer(intent:Intent, creature:Creature, reproductive_buffer:ReproductiveBuffer) -> None:
    if intent.intent == IntentActs.FIND_MATCH:
        reproductive_buffer.registry(ReproductiveDesire(creature.id, creature.genome.core.id))
    
