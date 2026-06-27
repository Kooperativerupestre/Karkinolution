
from dataclasses import dataclass
from organism.identity import EntityTypes
from organism.stats import Energy
from organism.creatures import Creature
from organism.ontology import FoodHint
from core.coord import Coord
from decisions.perception import Perception, PerceivedCreature
from systems.physics import MovementSystem, AttackSystem


CORPSE_FOOD_YIELD = 0.8


@dataclass(frozen=True)
class FoodOption:
    coord: Coord
    food_hint: FoodHint
    energy_gain: float
    energy_cost: int  | float
    diet_bias: float


class MetabolismSystem:

    @staticmethod
    def get_food_yield(energy: float, food_type: FoodHint) -> float:
        if food_type == FoodHint.CORPSE:
            return energy * CORPSE_FOOD_YIELD
        return energy

    @staticmethod
    def collect_options(perception: Perception, creature: Creature) -> list[FoodOption]:
        options: list[FoodOption] = []
        diet = creature.genome.metabolism.diet

        for coord in perception.iter_keys:
            block = perception.get(coord)
            energy_cost = MovementSystem.calculate_cost_to_move(perception, coord, creature)
            
            # corpse
            if block.get_entity_type() == EntityTypes.CORPSE:
                assert isinstance(block.entity, PerceivedCreature)

                raw_energy = block.entity.energy.value
                energy_gain = MetabolismSystem.get_food_yield(raw_energy, FoodHint.CORPSE)

                options.append(FoodOption(
                    coord=coord,
                    food_hint=FoodHint.CORPSE,
                    energy_gain=energy_gain,
                    energy_cost=energy_cost,
                    diet_bias=diet.corpse_score
                ))

            # grass
            if block.cell.food is not None:
                raw_energy = block.cell.food.value
                energy_gain = MetabolismSystem.get_food_yield(raw_energy, FoodHint.GRASS)

                options.append(FoodOption(
                    coord=coord,
                    food_hint=FoodHint.GRASS,
                    energy_gain=energy_gain,
                    energy_cost=energy_cost,
                    diet_bias=diet.grass_score
                ))

            # creature target
            if block.get_entity_type() == EntityTypes.CREATURE:
                assert isinstance(block.entity, PerceivedCreature)

                energy_cost += AttackSystem.calculate_cost_to_kill(creature, block.entity)

                raw_energy = block.entity.energy.value
                energy_gain = MetabolismSystem.get_food_yield(raw_energy, FoodHint.TARGET)

                options.append(FoodOption(
                    coord=coord,
                    food_hint=FoodHint.TARGET,
                    energy_gain=energy_gain,
                    energy_cost=energy_cost,
                    diet_bias=diet.target_score
                ))

        return options

    @staticmethod
    def score(option: FoodOption, hungry: float) -> float:
        # hunger low = neutral prefference
        diet_effective = 1 + (1 - hungry) * (option.diet_bias - 1)

        effective_gain = option.energy_gain * diet_effective
        return effective_gain - option.energy_cost

    @staticmethod
    def choose_best(perception: Perception, creature: Creature) -> FoodOption | None:
        options = MetabolismSystem.collect_options(perception, creature)

        if not options:
            return None

        return max(
            options,
            key=lambda opt: MetabolismSystem.score(opt, creature.hungry)
        )

    @staticmethod
    def eat(creature: Creature, energy: Energy, food_hint:FoodHint) -> None:
        needed = creature.needed_energy
        extracted = MetabolismSystem.get_food_yield(energy.value, food_hint)
        amount = min(needed, extracted)

        energy.sub(amount)
        creature.energy.add(amount)
    