from dataclasses import dataclass
from organism.creatures import Creature, PregnantUterus, EmptyUterus, Gestation
from organism.ontology import Gender
from organism.identity import Id
from organism.genetics import Genome
from organism.stats import Energy, LimitedValue, check_energy
from core.error import GenderError, DifferentSpeciesError, AlreadyPregnantError, ReproductiveError
from random import choices
from typing import Callable
from decisions.perception import PerceivedCreature

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
        
        if creature.pregnant:        
            if creature.uterus.all_children_borned: # type: ignore
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
    def can_reproduce(A:Creature, B:PerceivedCreature) -> bool:
        if B.gender is not None:
            return A.reproductively_capable and A.gender == Gender.other_sex(B.gender) and B.reproductive_capacity and B.specie_id == A.genome.core.id
        return False
    @staticmethod
    def return_parents(A:Creature, B:Creature) -> Parents:
        a_gender = A.gender
        b_gender = B.gender

        if a_gender == b_gender:
            raise ValueError('Parents must have two different genders')
        
        female = A.id if a_gender == Gender.FEMALE else B.id
        male = B.id if b_gender == Gender.MALE else A.id
        return Parents(female, male)
    
