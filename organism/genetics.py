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

class CreatureGenomes:
    g_metabolism: dict[CreatureTypes, MetabolismGenome] = {}
    g_body: dict[CreatureTypes, BodyGenome] = {}
    g_reproduction: dict[CreatureTypes, ReproductionGenome] = {}
    g_core: dict[CreatureTypes, CoreGenome] = {}

    def exists_creature_type(self, c_t: CreatureTypes) -> bool:
        return (
            c_t in self.g_metabolism
            and c_t in self.g_body
            and c_t in self.g_reproduction
            and c_t in self.g_core
        )

    def get_type_genome(self, c_t: CreatureTypes) -> Genome:
        if not self.exists_creature_type(c_t):
            raise ValueError(f'Creature type {c_t} does not exist in all genomes')
        return Genome(
            self.g_metabolism[c_t],
            self.g_body[c_t],
            self.g_reproduction[c_t],
            self.g_core[c_t]
        )

    def add_g_metabolism(self, g_metabolism: MetabolismGenome, c_t: CreatureTypes) -> None:
        self.g_metabolism[c_t] = g_metabolism

    def add_g_body(self, g_body: BodyGenome, c_t: CreatureTypes) -> None:
        self.g_body[c_t] = g_body

    def add_g_reproduction(self, g_reproduction: ReproductionGenome, c_t: CreatureTypes) -> None:
        self.g_reproduction[c_t] = g_reproduction

    def add_g_core(self, g_core: CoreGenome, c_t: CreatureTypes) -> None:
        self.g_core[c_t] = g_core
    

def populate_default_genomes(creatures_genomes: CreatureGenomes) -> None:
    # CROCODILE
    creatures_genomes.add_g_metabolism(MetabolismGenome(
        max_hungry=0.80,
        diet=Diet(0.58, 0.085, 0.85),
        mass=1.25,
        energy_limit=100
    ), CreatureTypes.CROCODILE)
    creatures_genomes.add_g_body(BodyGenome(
        life_limit=100,
        strength=27,
        max_age=50
    ), CreatureTypes.CROCODILE)
    creatures_genomes.add_g_reproduction(ReproductionGenome(
        reproduction_cost=40,
        extra_reproduction_multiplier=1.2,
        fertility_limit=5,
        gestation_time=5
    ), CreatureTypes.CROCODILE)
    creatures_genomes.add_g_core(CoreGenome(
        capabilities={MoveActions.WALK, MoveActions.SWIMM},
        vision_radius=Coord(4, 4),
        behavior=Temperament.AGGRESSIVE,
        id=CreatureTypes.CROCODILE
    ), CreatureTypes.CROCODILE)

    # CRAB
    creatures_genomes.add_g_metabolism(MetabolismGenome(
        max_hungry=0.65,
        diet=Diet(0.5, 0.35, 0.15),
        mass=1.1,
        energy_limit=40
    ), CreatureTypes.CRAB)
    creatures_genomes.add_g_body(BodyGenome(
        life_limit=30,
        strength=7.5,
        max_age=30
    ), CreatureTypes.CRAB)
    creatures_genomes.add_g_reproduction(ReproductionGenome(
        reproduction_cost=5,
        extra_reproduction_multiplier=1.2,
        fertility_limit=3,
        gestation_time=4
    ), CreatureTypes.CRAB)
    creatures_genomes.add_g_core(CoreGenome(
        capabilities={MoveActions.WALK},
        vision_radius=Coord(3, 3),
        behavior=Temperament.NEUTRAL,
        id=CreatureTypes.CRAB
    ), CreatureTypes.CRAB)

    # FISH
    creatures_genomes.add_g_metabolism(MetabolismGenome(
        max_hungry=0.33,
        diet=Diet(0.2, 0.75, 0.15),
        mass=1.0,
        energy_limit=10
    ), CreatureTypes.FISH)
    creatures_genomes.add_g_body(BodyGenome(
        life_limit=9,
        strength=1,
        max_age=10
    ), CreatureTypes.FISH)
    creatures_genomes.add_g_reproduction(ReproductionGenome(
        reproduction_cost=1.9,
        extra_reproduction_multiplier=1.0,
        fertility_limit=1,
        gestation_time=2
    ), CreatureTypes.FISH)
    creatures_genomes.add_g_core(CoreGenome(
        capabilities={MoveActions.SWIMM},
        vision_radius=Coord(2, 4),
        behavior=Temperament.PASSIVE,
        id=CreatureTypes.FISH
    ), CreatureTypes.FISH)

    # HIPPOPOTAMUS
    creatures_genomes.add_g_metabolism(MetabolismGenome(
        max_hungry=0.70,
        diet=Diet(0.2, 0.7, 0.1),
        mass=1.6,
        energy_limit=85
    ), CreatureTypes.HIPPOPOTAMUS)
    creatures_genomes.add_g_body(BodyGenome(
        life_limit=150,
        strength=20,
        max_age=60
    ), CreatureTypes.HIPPOPOTAMUS)
    creatures_genomes.add_g_reproduction(ReproductionGenome(
        reproduction_cost=40,
        extra_reproduction_multiplier=1.3,
        fertility_limit=4,
        gestation_time=5
    ), CreatureTypes.HIPPOPOTAMUS)
    creatures_genomes.add_g_core(CoreGenome(
        capabilities={MoveActions.WALK, MoveActions.SWIMM},
        vision_radius=Coord(3, 3),
        behavior=Temperament.TERRITORIAL,
        id=CreatureTypes.HIPPOPOTAMUS
    ), CreatureTypes.HIPPOPOTAMUS)


creatures_genomes = CreatureGenomes()
populate_default_genomes(creatures_genomes)