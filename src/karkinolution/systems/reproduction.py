from dataclasses import dataclass
from random import choices, uniform
from typing import Callable

from karkinolution.utils.k_random import choice_bool

from karkinolution.core.error import (
    AlreadyPregnantError,
    DifferentSpeciesError,
    GenderError,
    ReproductiveError,
)

from karkinolution.decisions.perception import PerceivedCreature

from karkinolution.organism.creatures import (
    Creature,
    EmptyUterus,
    Gestation,
    PregnantUterus,
)
from karkinolution.organism.genetics import Genome
from karkinolution.organism.identity import Id
from karkinolution.organism.ontology import Gender
from karkinolution.organism.stats import (
    Energy,
    LimitedValue,
    check_energy,
)

@dataclass(frozen=True)
class BornData:
    genome:Genome
    initial_energy:Energy
    sociability:int | float


    

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
        return choice_bool(death_tax, 1 - death_tax)
    
    
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

        sociability = creature.sociability.value * uniform(0.7, 1.3)
        
        child = BornData(child_genome, Energy(child_energy(), child_genome.metabolism.energy_limit), sociability)
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
        if B.ontology.gender is not None:
            return A.reproductively_capable and A.gender == Gender.other_sex(B.ontology.gender) and B.body.reproductiv_capacity and B.ontology.specie == A.genome.core.id
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
    
