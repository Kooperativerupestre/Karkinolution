from dataclasses import dataclass
from enum import Enum, auto

from karkinolution.core.coord import Coord
from karkinolution.core.error import InsufficientEnergyError

from karkinolution.decisions.perception import Perception

from karkinolution.organism.creatures import (
    Creature,
    EntitiesRegistry,
)
from karkinolution.organism.identity import Id
from karkinolution.organism.stats import Energy

from karkinolution.systems.metabolism import (
    FoodHint,
    MetabolismSystem,
)
from karkinolution.systems.physics import (
    AttackSystem,
    MovementSystem,
)
from karkinolution.systems.reproduction import ReproductiveSystem

from karkinolution.terrain.map import TerrainMotor
from karkinolution.terrain.world import (
    Log,
    LogEntry,
    World,
)

@dataclass(frozen=True)
class MovePreset:
    new_coord:Coord
@dataclass(frozen=True)
class ReproducePreset:
    female:Id
    male:Id
@dataclass(frozen=True)
class AttackPreset:
    target:Id
@dataclass
class EatPreset:
    energy:Energy
    food_hint:FoodHint

class MoveOutputs(Enum):
    OK = auto()
    INSUFFICIENT_ENERGY = auto()
    CANNOT_GET_BEST_POSITION = auto()


class PresetExecutor:
    @staticmethod
    def execute_reproduction(preset:ReproducePreset, world:World) -> None:
        # ALIAS
        buffer = world.reproductive_buffer
        entities = world.entities
        log = world.log

        # CODE
        female = entities.get_creature(preset.female)
        male = entities.get_creature(preset.male)

        
        costs = ReproductiveSystem.reproduce(female, male)
        female.energy.sub(costs.female_cost)
        male.energy.sub(costs.male_cost)

        buffer.try_remove(female.id)
        buffer.try_remove(male.id)
        log.add(LogEntry(world.time, f'Creatures {female} & {male} reproduced'))

    @staticmethod
    def execute_eat(preset:EatPreset, creature:Creature, time:int, log:Log) -> None:
        MetabolismSystem.eat(creature, preset.energy, preset.food_hint)
        log.add(LogEntry(time, f'Creature {creature} ate'))
    @staticmethod
    def execute_move(preset:MovePreset, creature:Creature, perception:Perception, world:World) -> MoveOutputs:
        best_pos = MovementSystem.best_pos(creature, perception, preset.new_coord)

        if best_pos is None:
            return MoveOutputs.CANNOT_GET_BEST_POSITION
        
        
        cost = MovementSystem.calculate_cost_to_move(best_pos, creature, world.territory)
        if creature.energy.value < cost:
            return MoveOutputs.INSUFFICIENT_ENERGY
        TerrainMotor.move(creature.position, best_pos, world.entity_map, world.territory)
        creature.energy.sub(cost)
        creature.position = best_pos
        world.log.add(LogEntry(world.time, f'Creature {creature} moved to {best_pos} '))
        return MoveOutputs.OK

    @staticmethod
    def execute_attack(preset:AttackPreset, entities:EntitiesRegistry, creature:Creature, log:Log, time:int) -> None:
        target = entities.get_creature(preset.target)

        cost = AttackSystem.attack(creature, target)
        creature.energy.sub(cost)
        log.add(LogEntry(time, f'Creature {creature} attacked {target}'))
    @staticmethod
    def _unsafe_execute_preset(preset:AttackPreset | ReproducePreset | EatPreset | MovePreset, creature:Creature, world:World, perception:Perception) -> None:
        if isinstance(preset, AttackPreset):
            PresetExecutor.execute_attack(preset, world.entities, creature, world.log, world.time)
        elif isinstance(preset, MovePreset):
            PresetExecutor.execute_move(preset, creature, perception, world)
        elif isinstance(preset, EatPreset):
            PresetExecutor.execute_eat(preset, creature, world.time, world.log)
        elif isinstance(preset, ReproducePreset):
            PresetExecutor.execute_reproduction(preset, world)
    @staticmethod
    def execute_preset(preset:AttackPreset | ReproducePreset | EatPreset | MovePreset, creature:Creature, world:World, perception:Perception) -> None:
        try:
            PresetExecutor._unsafe_execute_preset(preset, creature, world, perception)
        except InsufficientEnergyError:
            pass
        