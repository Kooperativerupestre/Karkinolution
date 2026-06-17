from organism.identity import gen_id, EntityTypes, Id
from organism.stats import Energy, Age


from organism.creatures import Creature, Corpse, PregnantUterus
from organism.ontology import Diet, FoodHint, FoodTarget

from core.coord import Coord
from decisions.perception import Perception
from systems.physics import MovementSystem


from typing import Iterable


 




class MetabolismSystem:
    #   PIPELINE
    #   evaluate -> coord = best coord
    #   food types = decide_food_types(coord, perception)
    #   chose best food type(coord, food types)

    @staticmethod
    def score_food(creature:Creature, coord_block:Coord, perception:Perception) -> float:
        food_score = 0

        cell_creature = perception.creature_block.cell
        next_block = perception.get(coord_block)
        diet_effective = 1 - creature.hungry

        if next_block.get_entity_type() == EntityTypes.CREATURE and next_block.entity.specie_id != creature.genome.core.id: # type: ignore
            food_score += (creature.genome.metabolism.diet.target_score * (1 - diet_effective) - creature.hungry) * next_block.entity.energy.value # type: ignore
        elif next_block.get_entity_type() == EntityTypes.CORPSE:
            food_score += (creature.genome.metabolism.diet.corpse_score * (1 - diet_effective) - creature.hungry) * next_block.entity.energy.value # type: ignore

        if next_block.cell.is_edible:
            food_score += (creature.genome.metabolism.diet.grass_score * (1 - diet_effective) + creature.hungry) * next_block.cell.food.value # type: ignore
        
        food_score -= perception.coord.distance_to_other(coord_block) * MovementSystem.calculate_cost_to_move(next_block.cell, cell_creature, creature)
        return food_score

    @staticmethod
    def evaluate(creature:Creature, perception:Perception) -> Iterable[tuple[Coord, float]]:
        ev = {c: MetabolismSystem.score_food(creature, c, perception) for c in perception.iter_keys}
        return ev.items()
    @staticmethod
    def best_coord(evaluated:Iterable[tuple[Coord, float]]) -> Coord:
        return max(evaluated, key=lambda x: x[1])[0]
    @staticmethod
    def decide_food_types(coord:Coord, perception:Perception) -> list[FoodHint]:
        energies:list[FoodHint] = []

        block = perception.get(coord)

        if block.cell.is_edible:
            assert block.cell.food is not None
            energies.append(FoodHint.GRASS)
        if block.get_entity_type() == EntityTypes.CREATURE and block.entity.specie_id != perception.creature.specie_id: # type: ignore
            energies.append(FoodHint.TARGET)
        if block.get_entity_type() == EntityTypes.CORPSE:
            energies.append(FoodHint.CORPSE)
        return energies
    @staticmethod
    def chose_best_food_type(food_types:list[FoodHint], diet:Diet) -> FoodHint:
        weights = {
            FoodHint.CORPSE: diet.corpse_score,
            FoodHint.TARGET: diet.target_score,
            FoodHint.GRASS: diet.grass_score
        }

        return max(food_types, key=lambda x: weights[x])
    
    @staticmethod
    def find_food_target(creature:Creature, perception:Perception) -> FoodTarget | None:
        ev = MetabolismSystem.evaluate(creature, perception)
        coord = MetabolismSystem.best_coord(ev)
        food_types = MetabolismSystem.decide_food_types(coord, perception)
        if len(food_types) == 0:
            return None
        food_type = MetabolismSystem.chose_best_food_type(food_types, creature.genome.metabolism.diet)
        return FoodTarget(food_type, coord)
    
    @staticmethod
    def eat(creature:Creature, energy:Energy) -> None:
        needed_energy = creature.needed_energy

        cost = min(needed_energy, energy.value)
        energy.sub(cost)
        creature.energy.add(cost)
    
    
            
        
class DeathSystem:
    @staticmethod
    def generate_corpse(creature:Creature) -> Corpse:
        energy:float = creature.energy.value * 0.8
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            energy += creature.uterus.pregnancy_cost * 2

        decomposition_time = energy/creature.energy.limit * 7.5
        return Corpse(Energy(energy, energy), gen_id(), Age(0, decomposition_time), creature.position)
    @staticmethod
    def is_dead(creature:Creature) -> bool:
        if creature.life.value == 0:
            return True
        if creature.energy.value == 0:
            return True
        if creature.age.value >= creature.age.limit:
            return True
        return False
          
