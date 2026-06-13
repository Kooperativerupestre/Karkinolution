from organism.ontology import Temperament, Gender
from systems.physics import MovementSystem
from systems.biology import MetabolismSystem, FoodHint, FoodTarget
from decisions.perception import Perception, Analysis, PerceivedCreature, PerceivedCell
from organism.creatures import Creature
from decisions.actions import Intent, IntentActs
from systems.presets import MovePreset, ReproducePreset, EatPreset, AtackPreset
from random import choice
from organism.identity import Id, EntityTypes
from dataclasses import dataclass
from organism.genetics import CreatureTypes
from core.error import IdExistenceError, EntityTypeError
from organism.stats import LimitedValue


COURAGE_FACTOR = {
    Temperament.PASSIVE: 0.1,
    Temperament.NEUTRAL: 0.3,
    Temperament.AGGRESSIVE: 0.7,
    Temperament.TERRITORIAL: 1
}



TRADE_OFF = {
    Temperament.PASSIVE: 0.1,
    Temperament.NEUTRAL: 0.5,
    Temperament.AGGRESSIVE: 0.89,
    Temperament.TERRITORIAL: 1
}


@dataclass(frozen=True)
class ReproductiveDesire:
    creature_id:Id
    specie_id:CreatureTypes

class ReproductiveBuffer:
    def __init__(self):
        self.desires:dict[Id, ReproductiveDesire] = {}

    def require_first_by_specie(self, specie_id:CreatureTypes) -> None | ReproductiveDesire:
        # O(n)

        for desire in self.desires.values():
            if desire.specie_id == specie_id:
                return desire
    def get(self, id:Id) -> ReproductiveDesire:
        return self.desires[id]
    
    def try_remove(self, id:Id) -> None:
        if id in self.desires:
            del self.desires[id]
    def add(self, desire:ReproductiveDesire) -> None:
        id = desire.creature_id
        if id in self.desires:
            raise IdExistenceError('ID {} already exists'.format(id.id))
        self.desires[id] = desire
    def registry(self, desire:ReproductiveDesire) -> None:
        if id not in self.desires:
            self.desires[desire.creature_id] = desire

def resolve_atack(perception:Perception, creature:Creature) -> Id | None:
    temperament = creature.genome.core.behavior
    coord_creature = perception.coord

    if temperament is Temperament.NEUTRAL or temperament is Temperament.PASSIVE:
        if creature.last_atack is None:
            return None
        else:
            return creature.last_atack.atacker_id
    elif temperament is Temperament.AGGRESSIVE:
        if creature.last_atack is not None:
            return creature.last_atack.atacker_id
        else:
            other_species = Analysis.other_species(perception)
            if len(other_species) == 0:
                return None
            return perception.get(Analysis.near_coord(other_species, coord_creature=coord_creature)).creature.id # type: ignore
        
    elif temperament is Temperament.TERRITORIAL:
        if creature.last_atack is not None:
            return creature.last_atack.atacker_id
        else:
            other_species = Analysis.other_species(perception)
            other_species = [c for c in other_species if c.x <= 2 + coord_creature.x and c.y <= 2 + coord_creature.y]
            if len(other_species) == 0:
                return None
            return perception.get(Analysis.near_coord(other_species, coord_creature=coord_creature)).creature.id # type: ignore
        
def score_atack(creature:Creature, target:PerceivedCreature) -> float:
    physical_factor = LimitedValue((creature.hungry + creature.life.ratio + creature.energy.ratio)/3, 1) # [0, 1]
    fear = LimitedValue(0, 1)
    fear.sub(creature.physical_ratio - target.physical_ratio)

    fear.sub(COURAGE_FACTOR[creature.genome.core.behavior])
    physical_factor.add(TRADE_OFF[creature.genome.core.behavior])

    total = (fear.value + physical_factor.value)/2 # [0, 1]
    
    if creature.pregnant:
        total -= 0.2
    return total



class EvaluateActions:
    @staticmethod
    def score_eat(creature:Creature) -> float:
        if creature.hungry < creature.genome.metabolism.max_hungry:
            return 0.0

        factor = creature.hungry * 2

        if creature.gender is Gender.FEMALE:
            if creature.uterus.pregnant: # type: ignore
                factor += creature.pregnancy_factor
        return factor
    @staticmethod
    def score_reproduce(creature:Creature) -> float:
        factor = creature.reproductive_maturity * creature.reproductive_factor
        

        if not creature.fertility.reproductive_capability:
            factor *= 0.2
        return factor

class DecideIntention:
    @staticmethod
    def decide(creature:Creature, reproductive_buffer:ReproductiveBuffer) -> Intent:
        acts:dict[IntentActs, float] = {}
        acts[IntentActs.NOTHING] = 0.8
        acts[IntentActs.FIND_FOOD] = EvaluateActions.score_eat(creature)
        if creature.gender is Gender.MALE:
            acts[IntentActs.FIND_MATCH] =  EvaluateActions.score_reproduce(creature)
        elif creature.gender == Gender.FEMALE and not creature.pregnant:
            acts[IntentActs.FIND_MATCH] = EvaluateActions.score_reproduce(creature)
        




        chose = max(acts, key=lambda x: acts[x])
        intent = Intent(chose)
        if intent.intent == IntentActs.FIND_MATCH:
            reproductive_buffer.registry(ReproductiveDesire(creature.id, creature.genome.core.id))
        return intent



class Planner:
    @staticmethod
    def plan_find_food_intent(perception:Perception, creature:Creature) -> MovePreset | AtackPreset | EatPreset | None:
        weights = {}
        food_target = MetabolismSystem.find_food_target(creature, perception)
        coord_creature = perception.coord


        if food_target is None:
            return None
        
        food_coord = food_target.coord

        if food_coord == coord_creature:
            energy:Energy = perception.get(food_coord).cell.food # type: ignore
            weights[EatPreset(energy)] = 1

        if coord_creature.distance_exceeds_one(food_coord):
            weights[MovePreset(food_coord)] = 0.8
        
        block = perception.get(food_coord)

        if coord_creature.distance_to_other(food_coord) == 1:
            if food_target.food_hint == FoodHint.CORPSE:
                assert block.entity is not None
                energy = block.entity.energy
                weights[EatPreset(energy)] = 1
            elif food_target.food_hint == FoodHint.TARGET:
                assert block.entity is not None
                weights[AtackPreset(block.entity.identity)] = score_atack(creature, block.entity)

            weights[MovePreset(food_coord)] = 0.8
        if len(weights) == 0:
            return None
        return max(weights, key=lambda x: weights[x])


        
    
    @staticmethod
    def plan_find_match_intent(perception:Perception, creature:Creature) -> MovePreset | None:
        other_sex = Gender.other_sex(creature.gender)
        # other_sex() returns the opposite sex of the creature


        sames_specie = Analysis.same_species(perception, predicate=lambda x: x.entity.can_reproduce) # type: ignore

        if len(sames_specie) == 0:
            return None

        near_coord = Analysis.near_coord(sames_specie, perception.coord)
    
        if perception.coord.distance_exceeds_one(near_coord):
            return MovePreset(near_coord)

        return None
