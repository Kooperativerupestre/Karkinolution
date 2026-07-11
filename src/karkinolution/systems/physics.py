from math import ceil
from typing import Callable

from karkinolution.core.coord import Coord
from karkinolution.core.error import NonMotileError

from karkinolution.decisions.actions import MoveActions
from karkinolution.decisions.perception import (
    PerceivedBlock,
    PerceivedCreature,
    Perception,
    PerceptionAnalyser
)

from karkinolution.organism.creatures import Creature
from karkinolution.organism.ontology import AttackedEvent
from karkinolution.organism.stats import check_energy

from karkinolution.terrain.map import Territory
from karkinolution.terrain.cell import (
    Damage,
    MovimentCost
)

class SpatialSystem:
    @staticmethod
    def can_move(block:PerceivedBlock, capabilitys:set[MoveActions]) -> bool:
        return block.cell.required_capabilities.issubset(capabilitys) and not block.has_entity
    @staticmethod
    def can_go(block:PerceivedBlock, capabilitys:set[MoveActions]) -> bool:
        return block.cell.required_capabilities.issubset(capabilitys)
    @staticmethod
    def get_effects(coord:Coord, perception:Perception, territory:Territory, creature:Creature) -> list[Callable[[Creature], None]]:
        effects:list[Callable[[Creature], None]] = []
        cell = territory.get(coord)
        block = perception.get(coord)


        if cell.component_is_in(Damage):
            effects.append(lambda x: creature.life.sub(cell.get_component(Damage).damage))
        
        if not SpatialSystem.can_move(block, creature.genome.core.capabilities):
            effects.append(lambda x: x.life.sub(x.life.value*0.10))
        return effects
    

    @staticmethod
    def apply_effects(effects:list[Callable[[Creature], None]], creature:Creature) -> None:
        for f in effects:
            f(creature)

        
class MovementSystem:
    @staticmethod
    def calculate_cost_to_move(new_coord:Coord, creature:Creature, territory:Territory) -> float:

        cell = territory.get(new_coord)

        if not cell.component_is_in(MovimentCost):
            raise NonMotileError

        cost:int | float = cell.get_component(MovimentCost).moviment_cost * creature.genome.metabolism.mass # type: ignore

        return cost * creature.genome.metabolism.mass # type: ignore
    

        
    @staticmethod
    def best_pos(creature:Creature, perception:Perception, new_coord:Coord) -> Coord | None:
        data = PerceptionAnalyser.neighbors_4(perception).iter
        moveble_coords:list[Coord] = []
        for c, b in data:
            if SpatialSystem.can_move(b, creature.genome.core.capabilities):
                moveble_coords.append(c)
        if len(moveble_coords) == 0:
            return None
        return min(moveble_coords, key=lambda x: x.distance_to_other(new_coord))
    @staticmethod
    def four_movable_coords(perception:Perception, creature:Creature) -> list[Coord]:
        four_neighbors = PerceptionAnalyser.neighbors_4(perception).iter

        valids:list[Coord] = []
        for c, b in four_neighbors:
            if SpatialSystem.can_move(b, creature.genome.core.capabilities):
                valids.append(c)
        return valids

    
class AttackSystem:
    # CALCS
    @staticmethod
    def calculate_cost_to_attack(creature:Creature) -> float:
        return 1.7 * creature.genome.metabolism.mass
    @staticmethod
    def calculate_attacks_to_kill(creature_strength:int | float, target_life:int | float) -> int:
        return ceil(target_life/creature_strength)

    @staticmethod
    def calculate_cost_to_kill(creature:Creature, target:PerceivedCreature) -> float:
        assert target.life is not None
        cost_atk = AttackSystem.calculate_cost_to_attack(creature)
        atks = AttackSystem.calculate_attacks_to_kill(creature.genome.body.strength, target.life)
        return cost_atk * atks
    # FUNCTIONS
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
