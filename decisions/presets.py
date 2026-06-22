from dataclasses import dataclass
from core.coord import Coord
from organism.stats import Energy
from organism.identity import Id
from organism.creatures import EntitiesRegistry, Creature
from systems.reproduction import ReproductiveSystem
from systems.physics import MovementSystem, AttackSystem
from decisions.perception import Perception
from systems.metabolism_system import MetabolismSystem, FoodHint
from map.world import World, Log
from systems.reproductivebuffer import ReproductiveBuffer

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

class PresetExecutor:
    @staticmethod
    def execute_reproduction(preset:ReproducePreset, entities:EntitiesRegistry, buffer:ReproductiveBuffer, log:Log) -> None:
        female = entities.get_creature(preset.female)
        male = entities.get_creature(preset.male)

        
        costs = ReproductiveSystem.reproduce(female, male)
        female.energy.sub(costs.female_cost)
        male.energy.sub(costs.male_cost)

        buffer.try_remove(female.id)
        buffer.try_remove(male.id)
        log.add(f'Creatures {female} & {male} reproduced')

    @staticmethod
    def execute_eat(preset:EatPreset, creature:Creature, log:Log) -> None:
        MetabolismSystem.eat(creature, preset.energy, preset.food_hint)
        log.add(f'Creature {creature} ate')
    @staticmethod
    def execute_move(preset:MovePreset, creature:Creature, perception:Perception, coord_creature:Coord, world:World, log:Log) -> None:
        best_pos = MovementSystem.best_pos(creature, perception, preset.new_coord)

        if best_pos is None:
            return None
        
        
        cost = MovementSystem.calculate_cost_to_move(perception, best_pos, creature)
        if creature.energy.value < cost:
            return None
        MovementSystem.move(creature, perception, best_pos, world.entity_map, world.territory)
        creature.energy.sub(cost)
        log.add(f'Creature {creature} moved to {best_pos} ')

    @staticmethod
    def execute_attack(preset:AttackPreset, entities:EntitiesRegistry, creature:Creature, log:Log) -> None:
        target = entities.get_creature(preset.target)

        cost = AttackSystem.attack(creature, target)
        creature.energy.sub(cost)
        log.add(f'Creature {creature} attacked {target}')
    @staticmethod
    def execute_preset(preset:AttackPreset | ReproducePreset | EatPreset | MovePreset, creature:Creature, world:World, perception:Perception) -> None:
        if isinstance(preset, AttackPreset):
            PresetExecutor.execute_attack(preset,world.entities, creature, world.log)
        elif isinstance(preset, MovePreset):
            PresetExecutor.execute_move(preset, creature, perception, perception.coord, world, world.log)
        elif isinstance(preset, EatPreset):
            PresetExecutor.execute_eat(preset, creature, world.log)
        elif isinstance(preset, ReproducePreset):
            PresetExecutor.execute_reproduction(preset, world.entities, world.reproductive_buffer, world.log)