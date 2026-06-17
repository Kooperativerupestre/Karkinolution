from __future__ import annotations
from enum import Enum, auto
from random import choice, uniform
from organism.ontology import Temperament, Diet
from decisions.actions import  MoveActions
from core.coord import Coord
from dataclasses import dataclass
from abc import ABC, abstractmethod


class CreatureTypes(Enum):
    CRAB = auto()
    FISH = auto()
    CROCODILE = auto()
    HIPPOPOTAMUS = auto()

    @staticmethod
    def choice() -> CreatureTypes:
        return choice([CreatureTypes.CRAB, CreatureTypes.FISH, CreatureTypes.CROCODILE, CreatureTypes.HIPPOPOTAMUS])
    



def gen_disturb(v_min:float, v_max:float) -> float:
    return uniform(v_min, v_max)
def gen_little_disturb() -> float:
    return gen_disturb(0.90, 1.10)
def gen_medium_disturb() -> float:
    return gen_disturb(0.86, 1.16)
def smooth_scramble(v1:int | float, v2:int | float) -> int | float:
    return (v1 + v2)/2 * gen_little_disturb()
def time_scramble(v1:int, v2:int) -> int:
    return round((v1 + v2)/2) + choice([-1, 0, 1])




class BaseGenome(ABC):
    
    @abstractmethod
    def scramble(self, other) -> BaseGenome:...


@dataclass(frozen=True)
class MetabolismGenome(BaseGenome):
    max_hungry:float
    diet:Diet
    mass:int | float
    energy_limit:int | float

    def scramble(self, other:MetabolismGenome) -> MetabolismGenome:
        return MetabolismGenome(
            smooth_scramble(self.max_hungry, other.max_hungry),
            self.diet.scramble(other.diet),
            self.mass * gen_little_disturb(),
            (self.energy_limit + other.energy_limit)/2 * gen_medium_disturb()
        )

@dataclass(frozen=True)
class ReproductionGenome(BaseGenome):
    reproduction_cost:int | float
    extra_reproduction_multiplier: int | float
    fertility_limit:int
    gestation_time:int
    
    def scramble(self, other:ReproductionGenome) -> ReproductionGenome:
        return ReproductionGenome(
            smooth_scramble(self.reproduction_cost, other.reproduction_cost),
            smooth_scramble(self.extra_reproduction_multiplier, other.extra_reproduction_multiplier),
            time_scramble(self.fertility_limit, other.fertility_limit),
            time_scramble(self.gestation_time, other.gestation_time)
        )
    
@dataclass(frozen=True)
class BodyGenome(BaseGenome):
    life_limit:int | float
    strength: int | float
    max_age: int

    def scramble(self, other:BodyGenome) -> BodyGenome:
        return BodyGenome(
            smooth_scramble(self.life_limit, other.life_limit),
            smooth_scramble(self.strength, other.strength),
            time_scramble(self.max_age, other.max_age)
        )

@dataclass(frozen=True)
class CoreGenome:
    capabilities:set[MoveActions]
    vision_radius:Coord
    behavior:Temperament
    id:CreatureTypes
  

@dataclass(frozen=True)
class Genome:
    metabolism:MetabolismGenome
    body:BodyGenome
    reproduction:ReproductionGenome
    core:CoreGenome

    def crossover(self, other:Genome) -> Genome:
        new_metabolism_g = self.metabolism.scramble(other.metabolism)
        new_body_g = self.body.scramble(other.body)
        new_reproduction_g = self.reproduction.scramble(other.reproduction)
        
        return Genome(
            new_metabolism_g,
            new_body_g,
            new_reproduction_g,
            self.core
        )

_creatures_metabolism_system = {
    CreatureTypes.CROCODILE: MetabolismGenome(
        max_hungry=0.80,
        diet=Diet(0.4, 0.1, 0.5),
        mass=1.25,
        energy_limit=100
    ),

    CreatureTypes.CRAB: MetabolismGenome(
        max_hungry=0.65,
        diet=Diet(0.5, 0.35, 0.15),
        mass=1.1,
        energy_limit=40
    ),

    CreatureTypes.FISH: MetabolismGenome(
        max_hungry=0.33,
        diet=Diet(0.2, 0.75, 0.15),
        mass=1.0,  # assumido inexistente antes, mantido coerente com padrão leve
        energy_limit=10
    ),

    CreatureTypes.HIPPOPOTAMUS: MetabolismGenome(
        max_hungry=0.70,
        diet=Diet(0.2, 0.7, 0.1),
        mass=1.6,  # implícito no original como criatura massiva, mas mantido proporcional
        energy_limit=85
    )
}

_creatures_body_system = {
    CreatureTypes.CROCODILE: BodyGenome(
        life_limit=100,
        strength=27,
        max_age=50
    ),

    CreatureTypes.CRAB: BodyGenome(
        life_limit=30,
        strength=7.5,
        max_age=30
    ),

    CreatureTypes.FISH: BodyGenome(
        life_limit=9,
        strength=1,
        max_age=10
    ),

    CreatureTypes.HIPPOPOTAMUS: BodyGenome(
        life_limit=150,
        strength=20,
        max_age=60
    )
}

_creatures_reproduction_system = {
    CreatureTypes.CROCODILE: ReproductionGenome(
        reproduction_cost=40,
        extra_reproduction_multiplier=1.2,
        fertility_limit=5,
        gestation_time=5
    ),

    CreatureTypes.CRAB: ReproductionGenome(
        reproduction_cost=5,
        extra_reproduction_multiplier=1.2,
        fertility_limit=3,
        gestation_time=4
    ),

    CreatureTypes.FISH: ReproductionGenome(
        reproduction_cost=1.9,
        extra_reproduction_multiplier=1.0,
        fertility_limit=1,
        gestation_time=2
    ),

    CreatureTypes.HIPPOPOTAMUS: ReproductionGenome(
        reproduction_cost=40,
        extra_reproduction_multiplier=1.3,
        fertility_limit=4,
        gestation_time=5
    )
}

_creatures_core_system = {
    CreatureTypes.CROCODILE: CoreGenome(
        capabilities={MoveActions.WALK, MoveActions.SWIMM},
        vision_radius=Coord(4, 4),
        behavior=Temperament.AGGRESSIVE,
        id=CreatureTypes.CROCODILE
    ),

    CreatureTypes.CRAB: CoreGenome(
        capabilities={MoveActions.WALK},
        vision_radius=Coord(3, 3),
        behavior=Temperament.NEUTRAL,
        id=CreatureTypes.CRAB
    ),

    CreatureTypes.FISH: CoreGenome(
        capabilities={MoveActions.SWIMM},
        vision_radius=Coord(2, 4),
        behavior=Temperament.PASSIVE,
        id=CreatureTypes.FISH
    ),

    CreatureTypes.HIPPOPOTAMUS: CoreGenome(
        capabilities={MoveActions.WALK, MoveActions.SWIMM},
        vision_radius=Coord(3, 3),
        behavior=Temperament.TERRITORIAL,
        id=CreatureTypes.HIPPOPOTAMUS
    )
}

_creatures_genomes = {
    ct: Genome(
        metabolism=_creatures_metabolism_system[ct],
        body=_creatures_body_system[ct],
        reproduction=_creatures_reproduction_system[ct],
        core=_creatures_core_system[ct],
    )
    for ct in CreatureTypes
}