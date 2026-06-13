from organism.identity import gen_id, EntityTypes, Id
from core.error import (NonPregnancyError, DifferentSpeciesError, AlreadyPregnantError, FinishedError,
                   GenderFemaleError, GenderMaleError, NonReproducibleError, CoordinateOccupiedError)
from random import choices
from organism.stats import LimitedValue, Energy, check_energy, Age


from organism.creatures import Uterus, Creature, EntitysRegistry, Corpse
from organism.genetics import Genome
from utils.namegenerator import gen_name
from organism.ontology import Gender, Diet, FoodHint, FoodTarget

from core.map import TerrainView, EntityMap, Territory
from core.coord import Coord
from decisions.perception import Perception
from dataclasses import dataclass
from systems.physics import MovementSystem

from typing import Iterable



@dataclass(frozen=True)
class ReproductionCost:
    female_cost:int | float
    male_cost:int | float

@dataclass(frozen=True)
class Parents:
    female:Id
    male:Id


class UterusSystem:
    @staticmethod
    def finish(uterus:Uterus) -> None:
        if not uterus.pregnant:
            raise NonPregnancyError('Uterus {} has no pregnancy to finish'.format(uterus))
        uterus.male_genome = None
        uterus.number_children = None

    @staticmethod
    def random_children_number() -> int:
        return choices([1, 2, 3], weights=[1/2, 1/4, 1/8], k=1)[0]
    @staticmethod
    def die_a_child(death_tax:float) -> bool:
        return choices([True, False], weights=[1-death_tax, death_tax], k=1)[0]
    
    @staticmethod
    def conceive(uterus:Uterus, male_genome:Genome) -> None:
        if male_genome.core.id != uterus.female_genome.core.id:
            raise DifferentSpeciesError(f'Male and Female genomes belong to different species. Male genome id {male_genome.core.id} != Female genome id {uterus.female_genome.core.id}')
        if uterus.pregnant:
            raise AlreadyPregnantError('Already pregnant uterus')
        uterus.male_genome = male_genome
        uterus.number_children = LimitedValue(0, UterusSystem.random_children_number())

    @staticmethod
    def have_child(uterus:Uterus) -> Creature | None:
        if not uterus.pregnant:
            raise NonPregnancyError('Uterus {} is not pregnant to give birth'.format(uterus))
        if uterus.all_children_borned:
            raise FinishedError('Pregnancy is already finished')
        
        if ReproductiveSystem.die_a_child(uterus.gestation.death_factor): # type: ignore
            return None

    
        uterus.number_children.add(1) # type: ignore
        
        child_genome = uterus.female_genome.scramble(uterus.male_genome) # type: ignore

        child_energy = lambda: uterus.birth_energy


        
        child = Creature(
            genome=child_genome,
            gender=Gender.choice(),
            name=gen_name(),
            initial_energy=child_energy(),
            id=gen_id(),
        )
        if uterus.all_children_borned:
            UterusSystem.finish(uterus)

        return child

    @staticmethod
    def pass_time(uterus:Uterus, creature:Creature):
        if uterus.pregnant: 
            creature.energy.sub(uterus.pregnancy_cost)
            uterus.gestation.value += 1 # type: ignore


            if uterus.all_children_borned:
                UterusSystem.finish(uterus)
    

class ReproductiveSystem:
    @staticmethod
    def reproduce(female:Creature, male:Creature) -> ReproductionCost:
        check_energy(female.energy, female.genome.reproduction.reproduction_cost)
        check_energy(male.energy, male.genome.reproduction.reproduction_cost)


        if female.gender is not Gender.FEMALE:
            raise GenderFemaleError('Creature {} must be female to reproduce'.format(female))
        assert female.uterus is not None
        if male.gender is not Gender.MALE:
            raise GenderMaleError('Creature {} must be male to reproduce'.format(male))
        

        if not female.fertility.reproductive_capability:
            raise NonReproducibleError('Creature {} has no reproductive capability'.format(female))
        
        
        UterusSystem.conceive(female.uterus, male.genome)
        female.fertility.zero()

        return ReproductionCost(female.genome.reproduction.reproduction_cost, male.genome.reproduction.reproduction_cost)

    @staticmethod
    def to_birth(female:Creature, new_coord:Coord, entity_map:EntityMap, territory:Territory, entitys:EntitysRegistry) -> Creature | None:
        if female.gender is not Gender.FEMALE:
            raise GenderFemaleError('Creature {} must be female to give birth'.format(female))
        assert female.uterus is not None
        if not female.pregnant: 
            raise NonPregnancyError('Creature {} must be pregnant to give birth'.format(female))
        if TerrainView.is_occupied(new_coord, entity_map):
            raise CoordinateOccupiedError('Coord {} must be unoccupied'.format(new_coord))

        child = UterusSystem.have_child(female.uterus)
        

        female.fertility.zero()
        return child
    @staticmethod
    def can_reproduce(A:Creature, B:Creature) -> bool:
        return (A.genome.core.id == B.genome.core.id) and (A.reproductively_capable and B.reproductively_capable)
    @staticmethod
    def return_parents(A:Creature, B:Creature) -> Parents:
        a_gender = A.gender
        b_gender = B.gender

        if a_gender == b_gender:
            raise ValueError('Parents must have two different genders')
        
        female = A.id if a_gender == Gender.FEMALE else B.id
        male = B.id if b_gender == Gender.MALE else A.id
        return Parents(female, male)
    

 




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
            food_score += (creature.genome.diet.target_score * (1 - diet_effective) - creature.hungry) * block.entity.energy # type: ignore
        elif next_block.get_entity_type() == EntityTypes.CORPSE:
            food_score += (creature.genome.diet.corpse_score * (1 - diet_effective) - creature.hungry) * block.entity.energy # type: ignore

        if next_block.cell.is_edible:
            food_score += (creature.genome.diet.grass_score * (1 - diet_effective) + creature.hungry) * block.cell.food # type: ignore
        
        food_score -= perception.coord.distance_to_other(coord_block) * MovementSystem.calculate_cost_to_move(next_block.cell, cell_creature, creature)
        return food_score

    @staticmethod
    def evaluate(creature:Creature, perception:Perception) -> Iterable[tuple[Coord, float]]:
        ev = {c: MetabolismSystem.score_food(creature, c, perception) for c, b in perception.iter}
        return ev.items()
    @staticmethod
    def best_coord(evaluated:Iterable[tuple[Coord, float]]) -> Coord:
        return max(evaluated, key=lambda x: x[1])[0]
    @staticmethod
    def decide_food_types(coord:Coord, perception:Perception) -> list[FoodHint]:
        energies = []

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
    def find_food_target(creature:Creature, perception:Perception) -> FoodTarget:
        ev = MetabolismSystem.evaluate(creature, perception)
        coord = MetabolismSystem.best_coord(ev)
        food_type = MetabolismSystem.chose_best_food_type(MetabolismSystem.decide_food_types(coord, perception), creature.genome.metabolism.diet)
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
        energy = creature.energy.value * 0.8
        if creature.gender is Gender.FEMALE:
            if creature.uterus.pregnant: # type: ignore
                energy += creature.uterus.pregnancy_cost * 2 # type: ignore
        decomposition_time = energy/creature.energy.limit * 7.5
        return Corpse(Energy(energy, energy), gen_id(), Age(0, decomposition_time))
    @staticmethod
    def is_dead(creature:Creature) -> bool:
        if creature.life.value == 0:
            return True
        if creature.energy.value == 0:
            return True
        if creature.age.value >= creature.age.limit:
            return True
        return False
          
