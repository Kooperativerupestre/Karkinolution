from decisions.perception import PerceivedBlock

from organism.creatures import Creature
from organism.ontology import AttackedEvent

from core.error import NonMotileError
from core.coord import Coord
from map.map import TerrainMotor, EntityMap, Territory

from organism.stats import check_energy
from decisions.actions import MoveActions
from decisions.perception import Perception

from typing import Callable

class SpatialSystem:
    @staticmethod
    def can_move(block:PerceivedBlock, capabilitys:set[MoveActions]) -> bool:
        return block.cell.required_capabilities.issubset(capabilitys) and not block.has_entity
    @staticmethod
    def can_go(block:PerceivedBlock, capabilitys:set[MoveActions]) -> bool:
        return block.cell.required_capabilities.issubset(capabilitys)
    @staticmethod
    def get_effects(block:PerceivedBlock, creature:Creature) -> list[Callable[[Creature], None]]:
        effects:list[Callable[[Creature], None]] = []
        
        if (dmg :=block.cell.damage) is not None:
            effects.append(lambda x, dmg=dmg: x.life.sub(dmg))
        
        if not SpatialSystem.can_move(block, creature.genome.core.capabilities):
            effects.append(lambda x: x.life.sub(x.life.value*0.10))
        return effects
    

    @staticmethod
    def apply_effects(effects:list[Callable[[Creature], None]], creature:Creature) -> None:
        for f in effects:
            f(creature)
        
class MovementSystem:
    @staticmethod
    def calculate_cost_to_move(perception:Perception, new_coord:Coord, creature:Creature) -> float:
        # ALIAS
        next_cell = perception.get(new_coord).cell
        cell_creature = perception.creature_block.cell
        if next_cell.is_movable is False:
            raise NonMotileError('Cell {} is not motile'.format(next_cell))
        
        assert next_cell.movement_cost is not None
        assert cell_creature.movement_cost is not None
        cost = (next_cell.movement_cost + cell_creature.movement_cost) / 2
        return cost * creature.genome.metabolism.mass * perception.coord.distance_to_other(new_coord)
    @staticmethod
    def move(creature:Creature, perception:Perception, new_coord:Coord, entity_map:EntityMap, territory:Territory) -> float:
        cost = MovementSystem.calculate_cost_to_move(perception, new_coord, creature)

        check_energy(creature.energy, cost)
        TerrainMotor.move(creature.id, creature.position, new_coord, entity_map, territory)
        creature.position = new_coord
        return cost
        

    @staticmethod
    def best_pos(creature:Creature, perception:Perception, new_coord:Coord) -> Coord | None:
        data = perception.neighbors_4_require
        moveble_coords:list[Coord] = []
        for c, b in data:
            if b is not None and SpatialSystem.can_move(b, creature.genome.core.capabilities):
                moveble_coords.append(c)
        if len(moveble_coords) == 0:
            return None
        return min(moveble_coords, key=lambda x: x.distance_to_other(new_coord))


    
class AttackSystem:
    @staticmethod
    def attack(creature:Creature, target:Creature) -> float:
        check_energy(creature.energy, 1.5)
    
        attack_event = AttackedEvent(creature.id, creature.genome.body.strength)

        target.life.sub(creature.genome.body.strength)

        target.last_attack = attack_event
        return 1.5

    @staticmethod
    def distance_can_atack(coord_creature:Coord, coord_target:Coord) -> bool:
        return not coord_creature.distance_exceeds_one(coord_target)
