from dataclasses import dataclass
from random import uniform

from karkinolution.core.coord import Coord
from karkinolution.terrain.map import Territory

from karkinolution.decisions.perception import (
    PerceivedCreature,
    Perception,
    PerceivedBlock,
    PerceivedCorpse
)

from karkinolution.organism.creatures import Creature
from karkinolution.organism.identity import EntityTypes
from karkinolution.organism.ontology import (
    Diet,
    FoodHint
)
from karkinolution.organism.stats import Energy

from karkinolution.systems.physics import (
    AttackSystem,
    MovementSystem,
)


CORPSE_FOOD_YIELD = 0.8


@dataclass(frozen=True)
class FoodOption:
    coord: Coord
    food_hint: FoodHint
    energy_gain: float
    distance: float


class MetabolismSystem:

    @staticmethod
    def get_food_yield(energy: float, food_type: FoodHint) -> float:
        if food_type == FoodHint.CORPSE:
            return energy * CORPSE_FOOD_YIELD
        return energy
    @staticmethod
    def get_food_options(perception:Perception, coord:Coord) -> list[FoodOption]:
        food_options:list[FoodOption] = []
        block = perception.get(coord)

        distance = perception.coord.distance_to_other(coord)
        if block.cell.is_edible:
            assert block.cell.food is not None

            food_options.append(
                FoodOption(
                    coord,
                    FoodHint.GRASS,
                    energy_gain=block.cell.food.value,
                    distance=distance
                )
            )
        
        if block.has_corpse:
            assert isinstance(block.entity, PerceivedCorpse)
            food_options.append(
                FoodOption(
                    coord,
                    FoodHint.CORPSE,
                    energy_gain=block.entity.energy.value,
                    distance=distance
                )
            )
        if block.has_creature:
            assert isinstance(block.entity, PerceivedCreature)
            food_options.append(
                FoodOption(
                    coord,
                    FoodHint.TARGET,
                    energy_gain=block.entity.body.energy.value,
                    distance=distance
                )
            )
        return food_options

    @staticmethod
    def diet_effective(diet_bias:float | int, hungry:float) -> float:
        return 1 + (1 - hungry) * (diet_bias - 1)


    @staticmethod
    def eat(creature: Creature, energy: Energy, food_hint:FoodHint) -> None:
        needed = creature.needed_energy

        if creature.pregnant:
            needed *= uniform(0.95, 1.05)
        else:
            needed *= uniform(0.90, 1.10)
        extracted = MetabolismSystem.get_food_yield(energy.value, food_hint)
        amount = min(needed, extracted)

        energy.sub(amount)
        creature.energy.add(amount)
    