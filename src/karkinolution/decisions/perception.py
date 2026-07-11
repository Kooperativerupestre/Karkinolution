from __future__ import annotations

from dataclasses import dataclass, replace
from math import sqrt
from typing import Callable, Iterable

from karkinolution.core.coord import Coord
from karkinolution.core.error import CoordinateNotFoundError

from karkinolution.decisions.actions import MoveActions

from karkinolution.organism.creatures import (
    Corpse,
    Creature,
    EntitiesRegistry,
)
from karkinolution.organism.genetics import CreatureTypes
from karkinolution.organism.identity import EntityTypes, Id
from karkinolution.organism.ontology import Gender
from karkinolution.organism.stats import Energy

from karkinolution.terrain.cell import (
    Cell,
    Damage,
    FoodState,
    MovimentCost,
    Properties,
)
from karkinolution.terrain.map import (
    EntityMap,
    Geometry,
    Territory,
)



@dataclass(frozen=True)
class PerceivedCreature:
    energy:Energy
    identity:Id
    life:int | float | None

    specie_id:CreatureTypes | None
    gender:Gender | None
    physical_ratio:float
    reproductive_capacity:bool


@dataclass(frozen=True)
class PerceivedCell:
    is_movable:bool
    is_edible:bool
    is_dangerous:bool
    
    movement_cost:int | float | None
    food:Energy | None
    required_capabilities: set[MoveActions]
    damage:int | float | None
@dataclass(frozen=True)
class ObserverCreature:
    energy_ratio:float
    life_ratio:float
    id:Id
    specie_id:CreatureTypes

@dataclass(frozen=True)
class PerceivedBlock:
    cell:PerceivedCell
    entity:PerceivedCreature | None
    distance:float

    @property
    def has_entity(self) -> bool:
        return self.entity is not None
    
    
    def get_entity_type(self) -> EntityTypes | None:
        return None if not self.has_entity else self.entity.identity.e_type # type: ignore
    @property
    def has_creature(self) -> bool:
        return self.get_entity_type() == EntityTypes.CREATURE
    @property
    def has_corpse(self) -> bool:
        return self.get_entity_type() == EntityTypes.CORPSE
    
@dataclass(frozen=True)
class Perception:
    pieces:dict[Coord, PerceivedBlock]
    creature:ObserverCreature
    coord:Coord
    max_distance:float

    def __iter__(self) -> Iterable[Coord]:
        return iter(self.pieces.keys())

    @property
    def iter(self) -> Iterable[tuple[Coord, PerceivedBlock]]:
        return self.pieces.items()
    @property
    def blocks(self) -> Iterable[PerceivedBlock]:
        return self.pieces.values()
    @property
    def coords(self) -> Iterable[Coord]:
        return self.pieces.keys()
    @property
    def size(self) -> int:
        return len(self.pieces)
    
    
    def get(self, coord:Coord) -> PerceivedBlock:
        try:
            return self.pieces[coord]
        except KeyError:
            raise CoordinateNotFoundError("coord {} was not found".format(coord))
    def try_get(self, coord:Coord) -> PerceivedBlock | None:
        return self.pieces.get(coord)
    @property
    def creature_block(self) -> PerceivedBlock:
        return self.get(self.coord)


class PerceptionAnalyser:
    @staticmethod
    def build_another_perception(old_perception:Perception, new_pieces:dict[Coord, PerceivedBlock]) -> Perception:
        return replace(old_perception, pieces=new_pieces)
    @staticmethod
    def find_predicate(perception:Perception, predicate:Callable[[PerceivedBlock, Coord], bool]) -> Perception:
        pieces:dict[Coord, PerceivedBlock] = {}

        for coord, block in perception.iter:
            if predicate(block, coord):
                pieces[coord] = block
        return PerceptionAnalyser.build_another_perception(perception, pieces)
    @staticmethod
    def find_predicate_coords(perception:Perception, predicate:Callable[[PerceivedBlock, Coord], bool]) -> set[Coord]:
        pieces:set[Coord] = set()

        for coord, block in perception.iter:
            if predicate(block, coord):
                pieces.add(coord)
        return pieces
    @staticmethod
    def build(perception:Perception, coords:set[Coord] | list[Coord] | tuple[Coord], predicate:Callable[[PerceivedBlock, Coord], bool] = lambda b, c: True) -> Perception:
        pieces:dict[Coord, PerceivedBlock] = {}

        for coord in coords:
            perceived_block = perception.try_get(coord)
            
            if perceived_block is not None and predicate(perceived_block, coord):
                pieces[coord] = perceived_block
        return PerceptionAnalyser.build_another_perception(perception, pieces)
    
    @staticmethod
    def neighbors_4(perception:Perception, include_self_coord:bool = False) -> Perception:
        coords = perception.coord.four_movements()

        if include_self_coord:
            coords.add(perception.coord)
        return PerceptionAnalyser.build(perception, coords)
    
    @staticmethod
    def neighbors_8(perception:Perception, include_self_coord:bool = False) -> Perception:
        coords = perception.coord.eight_movements()

        if include_self_coord:
            coords.add(perception.coord)
        return PerceptionAnalyser.build(perception, coords)

    @staticmethod
    def neighbors_x_y(perception:Perception, x_radius:int, y_radius:int) -> Perception: 
        return PerceptionAnalyser.find_predicate(perception, predicate=lambda _, c:  c.x - perception.coord.x > x_radius or c.y - perception.coord.y > y_radius or c == perception.coord)
    @staticmethod
    def near_coord(coord_creature:Coord, coords:set[Coord] | list[Coord] | tuple[Coord]) -> Coord:
        return min(coords, key=lambda x: coord_creature.distance_to_other(x))
    @staticmethod
    def get_area_in_radius_ratio(perception:Perception, radius_ratio:float) -> Perception:
        assert 0 <= radius_ratio<= 1
        return PerceptionAnalyser.find_predicate(perception, lambda _, c: c.distance_to_other(perception.coord) < radius_ratio * perception.max_distance )
    
    
class PerceptionPatterns:
    @staticmethod
    def empty_spaces(perception:Perception, predicate:Callable[[PerceivedBlock, Coord], bool] = lambda b, c: True) -> Perception:
        return PerceptionAnalyser.find_predicate(perception, predicate=lambda b, c: not b.has_entity and predicate(b, c))
    @staticmethod
    def corpses(perception:Perception, predicate:Callable[[PerceivedBlock, Coord], bool] = lambda b, c: True) -> Perception:
        return PerceptionAnalyser.find_predicate(perception, predicate=lambda b, c: b.has_corpse and predicate(b, c))
    
    @staticmethod
    def creatures(perception:Perception, predicate:Callable[[PerceivedBlock, Coord], bool] = lambda b, c: True) -> Perception:
        return PerceptionAnalyser.find_predicate(perception, predicate=lambda b, c: b.has_creature and predicate(b, c))
    @staticmethod
    def same_species(perception:Perception, predicate:Callable[[PerceivedBlock, Coord], bool] = lambda b, c: True) -> Perception:
        return PerceptionAnalyser.find_predicate(perception, predicate=lambda b, c: b.has_creature and b.entity.specie_id == perception.creature.specie_id and predicate(b, c))
    @staticmethod
    def other_species(perception:Perception, predicate:Callable[[PerceivedBlock, Coord], bool] = lambda b, c: True) -> Perception:
        return PerceptionAnalyser.find_predicate(perception, predicate=lambda b, c: b.has_creature and b.entity.specie_id != perception.creature.specie_id and predicate(b, c))


class Perceiver:
    @staticmethod
    def perceive_entity(creature:Creature) -> PerceivedCreature:
        return PerceivedCreature(
            creature.energy,
            creature.id,
            creature.life.value,
            creature.genome.core.id,
            creature.gender,
            creature.physical_ratio,
            creature.reproductively_capable
        )
    @staticmethod
    def perceive_corpse(corpse:Corpse) -> PerceivedCreature:
        return PerceivedCreature(
            corpse.energy,
            corpse.id,
            None,
            None,
            None,
            0,
            False
        )
    @staticmethod
    def perceive_cell(cell:Cell) -> PerceivedCell:
        is_edible = cell.property_is_in(Properties.EDIBLE)

        food: None | Energy = None
        if is_edible:
            food = cell.get_component(FoodState).food # type: ignore
        
        is_movable = True if len(cell.required_capabilities) > 0 else False

        movement_cost = None
        if is_movable:
            movement_cost = cell.get_component(MovimentCost).moviment_cost # type: ignore
        
        is_dangerous = cell.property_is_in(Properties.DANGEROUS)
        damage = None
        if is_dangerous:
            damage = cell.get_component(Damage).damage # type: ignore
        
        return PerceivedCell(
            is_movable,
            is_edible,
            cell.property_is_in(Properties.DANGEROUS),
            movement_cost,
            food,
            cell.required_capabilities,
            damage
        )
            
def perceive(creature:Creature, territory:Territory, entity_map:EntityMap,
             entities:EntitiesRegistry) -> Perception:
    vision_radius = creature.genome.core.vision_radius

    creature_observer = ObserverCreature(
        creature.energy.ratio,
        creature.life.ratio,
        creature.id,
        creature.genome.core.id,
    )
    data = Geometry.neighbors_x_y(creature.position, territory, entity_map, vision_radius, True)

    perceived:dict[Coord, PerceivedBlock] = {}

    for c, b in data.items():
        perceived_cell = Perceiver.perceive_cell(b.cell)

        id = b.id_creature

        perceived_creature = None
        
        if id is None:
            pass
        elif id.e_type == EntityTypes.CREATURE:
            perceived_creature = Perceiver.perceive_entity(entities.get_creature(id))
        elif id.e_type == EntityTypes.CORPSE:
            perceived_creature = Perceiver.perceive_corpse(entities.get_corpse(id))
        

        perceived[c] = PerceivedBlock(perceived_cell, perceived_creature, creature.position.distance_to_other(c))

    return Perception(perceived, creature_observer, creature.position, sqrt(creature.genome.core.vision_radius.x**2 + creature.genome.core.vision_radius.y**2))

        
