from __future__ import annotations
from enum import Enum, auto
from random import choice, choices, randint, uniform
from ontology import Temperament, Diet
from actions import Actions, MoveActions
from coord import Coord
from dataclasses import dataclass



class CreatureTypes(Enum):
    CRAB = auto()
    FISH = auto()
    CROCODILE = auto()
    HIPPOPOTAMUS = auto()

    @staticmethod
    def choice() -> CreatureTypes:
        return choice([CreatureTypes.CRAB, CreatureTypes.FISH, CreatureTypes.CROCODILE, CreatureTypes.HIPPOPOTAMUS])
    


@dataclass(frozen=True)
class Genome:
    max_hungry:float # [0, 1] # eat if hungry_now > max_hungry
    reproduction_cost:int | float
    extra_reproduction_multiplier: int | float # reproduce if energy_now * extra_reproduction_multiplier > reproduction_cost
    capabilities:set[Actions | MoveActions]
    vision_radius:Coord
    behavior:Temperament
    fertility_limit:int
    energy_limit:int | float
    life_limit:int | float
    gestation_time:int
    max_age:int
    strength:int | float
    diet:Diet
    id:CreatureTypes
    
    def capability_is_in(self, capability:Actions) -> bool:
        return capability in self.capabilities
    

    def scramble(self, other:Genome) -> Genome:
        return Genome(
            max_hungry=(self.max_hungry + other.max_hungry)/2,
            reproduction_cost=(self.reproduction_cost + other.reproduction_cost)/2,
            extra_reproduction_multiplier=self.extra_reproduction_multiplier * other.extra_reproduction_multiplier / 2,
            capabilities=self.capabilities | other.capabilities,
            vision_radius=self.vision_radius.add(other.vision_radius).div(2),
            behavior=choice([other.behavior, self.behavior]),
            fertility_limit=choice([other.fertility_limit, self.fertility_limit]),
            energy_limit=(self.energy_limit + other.energy_limit)/2,
            life_limit=(self.life_limit + other.life_limit)/2,
            gestation_time=max(1, round((self.gestation_time + other.gestation_time)/2)),
            max_age=round((self.max_age + other.max_age)/2),
            diet=self.diet,
            strength=(self.strength + other.strength)/2 * uniform(0.70, 1.30),
            id=self.id

        )
    def mutate(self) -> Genome:
        def gen_value() -> float:
            return uniform(0.92, 1.09)
        return Genome(
            min(max(self.max_hungry * gen_value(), 0), 1),
            self.reproduction_cost,
            self.extra_reproduction_multiplier * gen_value(),
            self.capabilities,
            self.vision_radius,
            self.behavior,
            self.fertility_limit,
            self.energy_limit * gen_value(),
            self.life_limit * gen_value(),
            self.gestation_time,
            self.max_age + choices([randint(-3, 3), 0], weights=(0.25, 0.75), k=1)[0],
            self.strength * gen_value()**2,
            self.diet,
            self.id
        )

creatures_genomes:dict[CreatureTypes, Genome] = {
    CreatureTypes.CROCODILE: Genome(
        max_hungry=0.80,
        reproduction_cost=40,
        extra_reproduction_multiplier=1.2,
        capabilities={MoveActions.WALK, MoveActions.SWIMM, Actions.ATACK},
        vision_radius=Coord(4, 4),
        behavior=Temperament.AGGRESSIVE,
        fertility_limit=5,
        energy_limit=100,
        life_limit=100,
        gestation_time=5,
        max_age=50,
        strength=27,
        diet=Diet.CARNIVORE,
        id=CreatureTypes.CROCODILE
    ),

    CreatureTypes.CRAB: Genome(
        max_hungry=0.65,
        reproduction_cost=5,
        extra_reproduction_multiplier=1.2,
        capabilities={MoveActions.WALK},
        vision_radius=Coord(3, 3),
        behavior=Temperament.NEUTRAL,
        fertility_limit=3,
        energy_limit=40,
        life_limit=30,
        gestation_time=4,
        max_age=30,
        strength=7.5,
        diet=Diet.HERBIVORE,
        id=CreatureTypes.CRAB
    ),
    
    CreatureTypes.FISH: Genome(
        max_hungry=0.33,
        reproduction_cost=1.9,
        extra_reproduction_multiplier=1,
        capabilities={MoveActions.SWIMM},
        vision_radius=Coord(2, 4),
        behavior=Temperament.PASSIVE,
        fertility_limit=1,
        energy_limit=10,
        life_limit=9,
        gestation_time=2,
        max_age=10,
        strength=1,
        diet=Diet.HERBIVORE,
        id=CreatureTypes.FISH
    ),

    CreatureTypes.HIPPOPOTAMUS: Genome(
        max_hungry=0.70,
        reproduction_cost=40,
        extra_reproduction_multiplier=1.3,
        capabilities={MoveActions.WALK, MoveActions.SWIMM},
        vision_radius=Coord(3, 3),
        behavior=Temperament.TERRITORIAL,
        fertility_limit=4,
        energy_limit=85,
        life_limit=150,
        gestation_time=5,
        max_age=60,
        strength=20,
        diet=Diet.HERBIVORE,
        id=CreatureTypes.HIPPOPOTAMUS
    )
}
