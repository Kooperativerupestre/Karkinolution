from organism.identity import gen_id, EntityTypes
from core.error import (NonPregnancyError, DifferentSpeciesError, AlreadyPregnantError, FinishedError,
                   GenderFemaleError, GenderMaleError, NonReproducibleError, CoordinateOccupiedError)
from random import choices
from organism.stats import LimitedValue, Energy, check_energy, Age


from organism.creatures import Uterus, Creature, EntitysRegistry, Corpse
from organism.genetics import Genome
from utils.namegenerator import gen_name
from organism.ontology import Gender, Diet

from core.world import WorldMotor
from core.map import TerrainView, EntityMap, Territory
from core.coord import Coord
from decisions.perception import Perception, Analysis, PerceivedBlock, PerceivedCell, PerceivedCreature
from dataclasses import dataclass
from enum import Enum, auto
from systems.physics import MovementSystem

class FoodHint(Enum):
    CORPSE = auto()
    OTHER_SPECIE = auto()
    SOLO = auto()

@dataclass
class FoodTarget:
    food_hint:FoodHint
    coord:Coord


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
        if male_genome.id != uterus.female_genome.id:
            raise DifferentSpeciesError(f'Male and Female genomes belong to different species. Male genome id {male_genome.id} != Female genome id {uterus.female_genome.id}')
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
    

class ReproductiveSystem:
    @staticmethod
    def reproduce(female:Creature, male:Creature) -> None:
        check_energy(female.energy, female.genome.reproduction_cost)
        check_energy(male.energy, male.genome.reproduction_cost)


        if female.gender is not Gender.FEMALE:
            raise GenderFemaleError('Creature {} must be female to reproduce'.format(female))
        if male.gender is not Gender.MALE:
            raise GenderMaleError('Creature {} must be male to reproduce'.format(male))
        

        if not female.fertility.reproductive_capability:
            raise NonReproducibleError('Creature {} has no reproductive capability'.format(female))
        
        ReproductiveSystem.conceive(female.uterus, male.genome) # type: ignore

        female.energy.sub(female.genome.reproduction_cost)
        male.energy.sub(male.genome.reproduction_cost)

    @staticmethod
    def to_birth(female:Creature, new_coord:Coord, entity_map:EntityMap, territory:Territory, entitys:EntitysRegistry) -> bool:
        if female.gender is not Gender.FEMALE:
            raise GenderFemaleError('Creature {} must be female to give birth'.format(female))
        if not female.uterus.pregnant: # type: ignore
            raise NonPregnancyError('Creature {} must be pregnant to give birth'.format(female))
        if TerrainView.is_occupied(new_coord, entity_map):
            raise CoordinateOccupiedError('Coord {} must be unoccupied'.format(new_coord))

        child = ReproductiveSystem.have_child(female.uterus) # type: ignore
        if child is None:
            return False
        WorldMotor.add_entity(territory, entity_map, child, new_coord, entitys)

        female.fertility.zero()
        return True


 




class MetabolismSystem:
    @staticmethod
    def is_edible(creature:Creature, block:PerceivedBlock) -> bool:
        entity_type = block.get_entity_type()
        if creature.genome.diet is Diet.CARNIVORE:
            return entity_type == EntityTypes.CORPSE
        elif creature.genome.diet is Diet.HERBIVORE:
            return block.cell.is_edible and MovementSystem.can_move(block, creature)
        else: # OMNIVORE
            return entity_type == EntityTypes.CORPSE or (block.cell.is_edible and MovementSystem.can_move(block, creature))


    @staticmethod
    def eat(creature:Creature, energy:Energy) -> None:
        needed_energy = creature.needed_energy

        cost = min(needed_energy, energy.value)
        energy.sub(cost)
        creature.energy.add(cost)

    @staticmethod
    def near_food(perception:Perception, creature:Creature) -> Coord | None:
        foods_coords = Analysis.find_predicate(perception, predicate=lambda x: MetabolismSystem.is_edible(creature, x))
        return min(foods_coords, key=perception.coord.distance_to_other) if len(foods_coords) > 0 else None
    @staticmethod
    def find_food_target(perception:Perception, creature:Creature) -> FoodTarget | None:
        diet = creature.genome.diet
        coord_creature = perception.coord
    

        if diet in [Diet.HERBIVORE, Diet.OMNIVORE] and MetabolismSystem.is_edible(creature, perception.creature_block):
            return FoodTarget(FoodHint.SOLO, coord_creature)
        
        near_food = MetabolismSystem.near_food(perception, creature)


        if near_food is None and diet is Diet.HERBIVORE:
            return None
        
        if near_food is None and diet in [Diet.OMNIVORE, Diet.CARNIVORE]:
            other_species = Analysis.other_species(perception)
            if len(other_species) == 0:
                return None
            near_food = Analysis.near_coord(other_species, coord_creature)
            return FoodTarget(FoodHint.OTHER_SPECIE, near_food)
        
        assert near_food is not None
        block = perception.get(near_food)

        entity_type = block.get_entity_type()
        if entity_type == EntityTypes.CORPSE:
            return FoodTarget(FoodHint.CORPSE, near_food)
        if entity_type == EntityTypes.CREATURE:
            return FoodTarget(FoodHint.OTHER_SPECIE, near_food)
        if block.cell.is_edible:
            return FoodTarget(FoodHint.SOLO, near_food)

            
        
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
          
    @staticmethod
    def resolve_death(creature:Creature, coord_creature:Coord, entity_map:EntityMap, territory:Territory, entitys:EntitysRegistry) -> None:
        corpse = DeathSystem.generate_corpse(creature)
        WorldMotor.delete_entity_by_coord(entity_map, coord_creature, creature.id, entitys)
        WorldMotor.add_entity(territory, entity_map, corpse, coord_creature, entitys)
    