from organism.identity import gen_id, EntityTypes, Id
from core.error import GenderError, DifferentSpeciesError, ReproductiveError, AlreadyPregnantError
from random import choices
from organism.stats import LimitedValue, Energy, check_energy, Age


from organism.creatures import Creature, Corpse, PregnantUterus, EmptyUterus, Gestation
from organism.genetics import Genome
from organism.ontology import Gender, Diet, FoodHint, FoodTarget

from core.coord import Coord
from decisions.perception import Perception
from dataclasses import dataclass
from systems.physics import MovementSystem

from typing import Iterable, Callable

@dataclass(frozen=True)
class BornData:
    genome:Genome
    initial_energy:Energy


    

@dataclass(frozen=True)
class ReproductionCost:
    female_cost:int | float
    male_cost:int | float

@dataclass(frozen=True)
class Parents:
    female:Id
    male:Id

def validate_female_gender(creature:Creature) -> None:
    if creature.gender != Gender.FEMALE:
        raise GenderError('Creature {} must be female'.format(creature))

    assert creature.uterus is not None
def validate_male_gender(creature:Creature) -> None:
    if creature.gender != Gender.MALE:
        raise GenderError('Creature {} must be male'.format(creature))

def validate_same_species(A:Genome, B:Genome) -> None:
    if A.core.id != B.core.id:
        raise DifferentSpeciesError('Genomes ({}, {}) belong to different species'.format(A, B))
    

class UterusSystem:
    # VALIDATIONS
    @staticmethod
    def validate_non_pregnant(creature:Creature) -> None:
        validate_female_gender(creature)

        if type(creature.uterus) == PregnantUterus:
            raise AlreadyPregnantError('Already pregnant uterus')
    @staticmethod
    def validate_pregnant(creature:Creature) -> None:
        validate_female_gender(creature)

        if not isinstance(creature.uterus, PregnantUterus):
            raise ReproductiveError('Uterus {} must be pregnant'.format(creature.uterus))


    # FUNCTIONS
    @staticmethod
    def finish(creature:Creature) -> None:
        UterusSystem.validate_pregnant(creature)
        creature.uterus = EmptyUterus()
    @staticmethod
    def random_children_number() -> int:
        return choices([1, 2, 3], weights=[1/2, 1/4, 1/8], k=1)[0]
    @staticmethod
    def die_a_child(death_tax:float) -> bool:
        return choices([True, False], weights=[1-death_tax, death_tax], k=1)[0]
    
    @staticmethod
    def conceive(creature:Creature, male_genome:Genome) -> None:
        UterusSystem.validate_non_pregnant(creature)
        validate_same_species(creature.genome, male_genome)
        
        
        number_children = LimitedValue(0, UterusSystem.random_children_number())
        gestation = Gestation(creature.genome.reproduction.gestation_time)
        creature.uterus = PregnantUterus(male_genome, gestation, number_children)


    @staticmethod
    def have_child(creature:Creature) -> BornData | None:
        UterusSystem.validate_pregnant(creature)
        assert isinstance(creature.uterus, PregnantUterus)


        if UterusSystem.die_a_child(creature.uterus.gestation.death_factor): 
            return None

    
        creature.uterus.number_children.add(1)
        
        child_genome = creature.genome.crossover(creature.uterus.male_genome)

        child_energy:Callable[[], float] = lambda: creature.uterus.birth_energy # type: ignore


        
        child = BornData(child_genome, Energy(child_energy(), child_genome.metabolism.energy_limit))
        if creature.uterus.all_children_borned:
            UterusSystem.finish(creature)

        return child

    @staticmethod
    def pass_time(creature:Creature):
        validate_female_gender(creature)
        
        if creature.uterus.pregnant:        # type: ignore
            if uterus.all_children_borned: # type: ignore
                UterusSystem.finish(creature)
            creature.uterus.gestation.value+=1; # type: ignore

    

class ReproductiveSystem:
    @staticmethod
    def validate_reproductive_capacity(creature:Creature) -> None:
        if not creature.reproductively_capable:
            raise ReproductiveError('Creature {} has no reproductive capability'.format(creature))
    @staticmethod
    def reproduce(female:Creature, male:Creature) -> ReproductionCost:
        check_energy(female.energy, female.genome.reproduction.reproduction_cost)
        check_energy(male.energy, male.genome.reproduction.reproduction_cost)


        validate_female_gender(female)
        validate_male_gender(male)
        

        ReproductiveSystem.validate_reproductive_capacity(female)
        ReproductiveSystem.validate_reproductive_capacity(male)
        UterusSystem.conceive(female, male.genome)
        female.fertility.zero()

        return ReproductionCost(female.genome.reproduction.reproduction_cost, male.genome.reproduction.reproduction_cost)

    @staticmethod
    def to_birth(female:Creature) -> BornData | None:
        UterusSystem.validate_pregnant(female)

        child = UterusSystem.have_child(female)
        

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
          
