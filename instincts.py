from organism.ontology import Temperament, Gender
from systems.physics import MovementSystem
from systems.biology import MetabolismSystem, FoodHint, FoodTarget
from perception import Perception, Analysis
from organism.creatures import Creature
from actions import Intent, IntentActs
from systems.pressets import MovePressets, ReproducePressets, EatPressets, AtackPressets
from random import choice
from identity import Id

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



def move_possibility(perception:Perception, creature:Creature) -> bool:
    return any(MovementSystem.can_move(b, creature) for b in perception.neighbors_9_blocks)

def reproduce_possibility(creature: Creature) -> bool:
    if creature.gender is Gender.FEMALE:
        if creature.uterus.pregnant: # type: ignore
            return False
        elif creature.age.ratio > 0.85:
            return False
    return (
        creature.reproductively_capable and
        creature.energy.value * creature.genome.extra_reproduction_multiplier > creature.genome.reproduction_cost
    )
def resolve_atack(perception:Perception, creature:Creature) -> Id | None:
    temperament = creature.genome.behavior
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
        



class EvaluateActions:
    @staticmethod
    def score_eat(creature:Creature) -> float:
        factor = creature.hungry * 2

        if creature.hungry > creature.genome.max_hungry:
            factor += 0.5

        if creature.gender is Gender.FEMALE:
            if creature.uterus.pregnant: # type: ignore
                factor += creature.pregnancy_factor
        print(f'FATOR {factor}')
        return factor
    @staticmethod
    def score_reproduce(creature:Creature) -> float:
        factor = creature.reproductive_maturity * creature.reproductive_factor
        

        if not creature.fertility.reproductive_capability:
            factor *= 0.2
        return factor

class DecideIntention:
    @staticmethod
    def decide(creature:Creature) -> Intent:
        acts:dict[IntentActs, float] = {}
        acts[IntentActs.NOTHING] = 0.8
        acts[IntentActs.FIND_FOOD] = EvaluateActions.score_eat(creature)
        if creature.gender is Gender.MALE:
            acts[IntentActs.FIND_MATCH] =  EvaluateActions.score_reproduce(creature)
        elif creature.gender is Gender.FEMALE:
            if not creature.uterus.pregnant: # type: ignore
                acts[IntentActs.FIND_MATCH] = EvaluateActions.score_reproduce(creature)




        chose = max(acts, key=lambda x: acts[x])
        intent = Intent(chose)
        return intent


class Planner:
    @staticmethod
    def random_move_presset(perception:Perception, creature:Creature) -> MovePressets | None:
        blocks = perception.neighbors_4_require
        moveble_blocks = {}
        for c, b in blocks:
            if b is not None and MovementSystem.can_move(b, creature):
                moveble_blocks[c] = b
        if len(moveble_blocks) == 0:
            return None
        
        block = choice(list(moveble_blocks.values()))
        coord = choice(list(moveble_blocks.keys()))
        move_type = MovementSystem.decide_movimentation(creature, block)
        assert move_type is not None, 'Move-type must not be None'

        return MovePressets(creature, perception.coord, coord, move_type) 
        
    @staticmethod
    def plan_find_food_intent(perception:Perception, creature:Creature) -> MovePressets | AtackPressets | EatPressets | None:
        food_target = MetabolismSystem.find_food_target(perception, creature)
        coord_creature = perception.coord


        if food_target is None:
            return None
        
        food_coord = food_target.coord

        if food_coord == coord_creature:
            energy:Energy = perception.get(food_coord).cell.get_component(FoodState).food # type: ignore
            return EatPressets(creature, energy)

        if coord_creature.distance_exceeds_one(food_coord):
            new_coord = MovementSystem.best_pos(creature, perception, food_coord)
            if new_coord is None:
                return None
            
            move_type = MovementSystem.decide_movimentation(creature, perception.get(new_coord))
            return MovePressets(creature, coord_creature, new_coord, move_type) # type: ignore
        
        block = perception.get(food_coord)

        if coord_creature.distance_to_other(food_coord) == 1:
            if creature.genome.behavior in [Temperament.AGGRESSIVE, Temperament.NEUTRAL] and food_target.food_hint is FoodHint.OTHER_SPECIE:
                return AtackPressets(creature, block.creature) # type: ignore
            
            move_type = MovementSystem.decide_movimentation(creature, block)
            if move_type is None:
                return None
            return MovePressets(creature, coord_creature, food_coord, move_type)
        return None


        
    
    @staticmethod
    def plan_find_match_intent(perception:Perception, creature:Creature) -> MovePressets | ReproducePressets | None:
        other_sex = Gender.other_sex(creature.gender)
        # other_sex() returns the opposite sex of the creature


        sames_specie = Analysis.same_species(perception, predicate=lambda x: x.creature.gender is other_sex) # type: ignore

        if len(sames_specie) == 0:
            return None

        near_coord = Analysis.near_coord(sames_specie, perception.coord)
        other_creature:Entity = perception.get(near_coord).creature # type: ignore

        if not perception.coord.distance_exceeds_one(near_coord):
            female = creature if creature.gender is Gender.FEMALE else other_creature
            male = creature if creature.gender is Gender.MALE else other_creature

            return ReproducePressets(female, male)

        new_coord = MovementSystem.best_pos(creature, perception, near_coord)
        # MovementSystem.best_pos guarantees that the coordinate is a valid destination for the creature


        if new_coord is None:
            return None

        movement_type = MovementSystem.decide_movimentation(creature, perception.get(new_coord))
        # A valid destination has already been verified


        return MovePressets(creature, perception.coord, new_coord, movement_type) # type: ignore
    
