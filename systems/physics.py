from decisions.perception import PerceivedBlock, PerceivedCell

from organism.creatures import Creature, Corpse
from organism.ontology import AtackedEvent

from core.error import NonMotileError, CoordinateOccupiedError
from core.coord import Coord
from core.map import TerrainView, TerrainMotor, EntityMap, Territory

from organism.stats import check_energy
from decisions.actions import MoveActions
from decisions.perception import Perception, Analysis

class MovementSystem:
    @staticmethod
    def can_move(block:PerceivedBlock, creature:Creature) -> bool:
        return block.cell.required_capabilities.issubset(creature.genome.core.capabilities) and not block.has_entity
    @staticmethod
    def calculate_cost_to_move(next_cell:PerceivedCell, cell_creature:PerceivedCell, creature:Creature) -> int | float:
        if next_cell.is_moveble is False:
            raise NonMotileError('Cell {} is not motile'.format(next_cell))
        assert next_cell.movement_cost is not None
        assert cell_creature.movement_cost is not None
        cost = (next_cell.movement_cost + cell_creature.movement_cost) / 2
        return cost * creature.genome.metabolism.mass
    @staticmethod
    def move(creature:Creature, coord_creature:Coord, new_coord:Coord, entity_map:EntityMap, territory:Territory) -> int:
        if TerrainView.is_occupied(new_coord, entity_map):
            raise CoordinateOccupiedError('Coord {} is occupied'.format(new_coord))
        if coord_creature == new_coord:
            raise ValueError('Coord of creature {} == New coord {}'.format(coord_creature, new_coord))

        check_energy(creature.energy, 1)
        TerrainMotor.move(creature.id, new_coord, entity_map, territory)

        return 1


        
    @staticmethod
    def decide_movimentation(creature:Creature, block:PerceivedBlock) -> MoveActions | None:
        if not MovementSystem.can_move(block, creature):
            return None
        else:
            return next(iter(block.cell.required_capabilities))
    @staticmethod
    def best_pos(creature:Creature, perception:Perception, new_coord:Coord) -> Coord | None:
        data = perception.neighbors_4_require
        moveble_coords = []
        for c, b in data:
            if b is not None and MovementSystem.can_move(b, creature):
                moveble_coords.append(c)
        return min(moveble_coords, key=lambda x: perception.coord.distance_to_other(x))
    
class AtackSystem:
    @staticmethod
    def atack(creature:Creature, target:Creature) -> float:
        check_energy(creature.energy, 1.5)
    
        atack_event = AtackedEvent(creature.id, creature.genome.body.strength)

        target.life.sub(creature.genome.body.strength)

        target.last_atack = atack_event
        return 1.5

    @staticmethod
    def distance_can_atack(coord_creature:Coord, coord_target:Coord) -> bool:
        return not coord_creature.distance_exceeds_one(coord_target)