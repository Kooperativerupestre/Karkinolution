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
from karkinolution.organism.ontology import Gender, Temperament
from karkinolution.organism.stats import Energy, LimitedValue

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
class PerceivedBody:
    energy:Energy
    life:int | float
    physical_ratio:float
    reproductiv_capacity:bool

@dataclass(frozen=True)
class PerceivedOntology:
    specie:CreatureTypes
    temperament:Temperament
    gender:Gender | None

@dataclass(frozen=True)
class PerceivedCreature:
    body:PerceivedBody
    ontology:PerceivedOntology
    id:Id

@dataclass(frozen=True)
class PerceivedCorpse:
    energy:Energy
    id:Id

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
    specie:CreatureTypes

@dataclass(frozen=True)
class PerceivedBlock:
    cell:PerceivedCell
    entity:PerceivedCreature | PerceivedCorpse | None
    distance:float

    @property
    def has_entity(self) -> bool:
        return self.entity is not None
    
    
    def get_entity_type(self) -> EntityTypes | None:
        return None if not self.has_entity else self.entity.id.e_type # type: ignore
    @property
    def has_creature(self) -> bool:
        return self.get_entity_type() == EntityTypes.CREATURE
        
    @property
    def has_corpse(self) -> bool:
        return self.get_entity_type() == EntityTypes.CORPSE
    # Alias
    # Creature = PerceivedCreature
    # Corpse = PerceivedCorpse

    # has_entity = true -> isinstance(entity, Creature) = true | isinstance(entity, Corpse) = true
    # has_creature = true -> isinstance(entity, Creature) = true
    # has_corpse = true -> isinstance(entity, Corpse) = true

    
class BlockProperties:
    @staticmethod
    def has_aggressive_creature(block:PerceivedBlock) -> bool:
        return block.has_creature and block.entity.ontology.temperament == Temperament.AGGRESSIVE # type: ignore
    @staticmethod
    def has_territorial_creature(block:PerceivedBlock) -> bool:
        return block.has_creature and block.entity.ontology.temperament == Temperament.TERRITORIAL # type: ignore
    @staticmethod
    def is_same_specie(other_creature:PerceivedCreature, creature:ObserverCreature) -> bool:
        return other_creature.specie == creature.specie  # type: ignore
    @staticmethod
    def is_different_specie(other:PerceivedCreature, creature:ObserverCreature) -> bool:
        return other.specie != creature.specie # type: ignore
    @staticmethod
    def is_more_strong(other:PerceivedCreature, creature:Creature) -> bool:
        return other.body.physical_ratio > creature.physical_ratio
    
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
        return PerceptionAnalyser.find_predicate(perception, predicate=lambda b, c: b.has_creature and b.entity.ontology.specie == perception.creature.specie and predicate(b, c)) # type: ignore
    @staticmethod
    def other_species(perception:Perception, predicate:Callable[[PerceivedBlock, Coord], bool] = lambda b, c: True) -> Perception:
        return PerceptionAnalyser.find_predicate(perception, predicate=lambda b, c: b.has_creature and b.entity.ontology.specie != perception.creature.specie and predicate(b, c)) # type: ignore


class Perceiver:
    @staticmethod
    def perceive_entity(creature:Creature) -> PerceivedCreature:
        body = PerceivedBody(
            creature.energy,
            creature.life.value,
            creature.physical_ratio,
            creature.reproductively_capable
        )
        ontology = PerceivedOntology(
            creature.genome.core.specie,
            creature.genome.core.temperament,
            creature.gender
        )

        return PerceivedCreature(
            body,
            ontology,
            creature.id
        )
    @staticmethod
    def perceive_corpse(corpse:Corpse) -> PerceivedCorpse:
        return PerceivedCorpse(
            corpse.energy,
            corpse.id
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

@dataclass
class Dangers:
    cell_danger:float
    creature_danger:float

    @property
    def accumulated_danger(self) -> float:
        return (self.cell_danger * 0.85 + self.creature_danger * 1.15)/2
    


TERRITORIAL_DANGER = 0.25
AGGRESSIVE_DANGER = 0.3
DANGEROUS_CELL_FACTOR = 0.18
ADJACENT_DANGER_WEIGHT = 0.4


@dataclass(frozen=True)
class DangerIndex:
    index:dict[Coord, Dangers]
    
    def get(self, coord:Coord) -> Dangers:
        return self.index[coord]

    def try_get(self, coord:Coord) -> Dangers | None:
        return self.index.get(coord)
    def add(self, coord:Coord, dangers:Dangers) -> None:
        if not coord in self.index:
            self.index[coord] = dangers

class DangerFactory:
    # all values must be in [0, 1]
    @staticmethod
    def get_creature_danger(block:PerceivedBlock, creature:Creature) -> float:
        score = LimitedValue(0, 1, -1)

        if block.has_creature:
            other:PerceivedCreature = block.entity # type: ignore
            if BlockProperties.has_territorial_creature(block):
                score.add(TERRITORIAL_DANGER)
            elif BlockProperties.has_aggressive_creature(block):
                score.add(AGGRESSIVE_DANGER)
        
            score.sub((creature.physical_ratio - other.body.physical_ratio)/2)
        return score.value
    @staticmethod
    def get_cell_danger(block:PerceivedBlock) -> float:
        score = LimitedValue(0, 1, -1)

        if block.cell.is_dangerous:
            score.add(DANGEROUS_CELL_FACTOR)
        return score.value
    @staticmethod
    def get_dangers(block:PerceivedBlock, creature:Creature) -> Dangers:
        return Dangers(
            DangerFactory.get_cell_danger(block),
            DangerFactory.get_creature_danger(block, creature)
        )
    @staticmethod
    def create_danger_index(perception:Perception, creature:Creature) -> DangerIndex:
        cache = DangerIndex({})
        danger_index = DangerIndex({})
        new_perception = PerceptionAnalyser.get_area_in_radius_ratio(perception, 0.75)


        for c, b in new_perception.iter:
            if c in cache.index:
                actual_dangers = cache.get(c)
            else:
                actual_dangers = DangerFactory.get_dangers(b, creature)
                cache.add(c, actual_dangers)
            
            four_coords = c.four_movements()

            average_dangers = Dangers(0, 0)
            count = 0
            for f_c in four_coords:
                if f_c not in perception.pieces:
                    continue

                if f_c not in cache.index:
                    dangers = DangerFactory.get_dangers(perception.get(f_c), creature)
                    cache.add(f_c, dangers)
                else:
                    dangers = cache.get(f_c)
                
                count += 1


                # sums
                average_dangers.cell_danger += dangers.cell_danger
                average_dangers.creature_danger += dangers.creature_danger

            if count > 0:
                average_dangers.cell_danger/=count
                average_dangers.creature_danger/=count

                average_dangers.cell_danger*=ADJACENT_DANGER_WEIGHT
                average_dangers.creature_danger*=ADJACENT_DANGER_WEIGHT

            new_dangers = Dangers(
                average_dangers.cell_danger + actual_dangers.cell_danger,
                average_dangers.creature_danger + actual_dangers.creature_danger
            )
            danger_index.add(c, new_dangers)
        return danger_index