from __future__ import annotations
from organism.genetics import CreatureTypes
from organism.ontology import Gender
from organism.creatures import Creature, Corpse, EntitysRegistry
from decisions.actions import MoveActions

from core.coord import Coord
from typing import Callable, Iterable
from organism.stats import Energy
from organism.identity import Id, EntityTypes
from dataclasses import dataclass
from core.error import CoordinateNotFoundError
from map.map import Territory, EntityMap, Geometry

from map.cell import Cell, FoodState, Properties, MovimentCost, Damage


@dataclass(frozen=True)
class PerceivedCreature:
    energy:Energy
    identity:Id

    specie_id:CreatureTypes | None
    gender:Gender | None
    can_reproduce:bool
    physical_ratio:float


@dataclass(frozen=True)
class PerceivedCell:
    is_moveble:bool
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

    @property
    def has_entity(self) -> bool:
        return self.entity is not None
    
    def get_entity_type(self) -> EntityTypes | None:
        return None if not self.has_entity else self.entity.identity.e_type # type: ignore


@dataclass(frozen=True)
class Perception:
    blocks:dict[Coord, PerceivedBlock]
    creature:ObserverCreature
    coord:Coord

    @property
    def iter(self) -> Iterable[tuple[Coord, PerceivedBlock]]:
        return self.blocks.items()
    @property
    def iter_keys(self) -> Iterable[Coord]:
        return self.blocks.keys()
    @property
    def iter_values(self) -> Iterable[PerceivedBlock]:
        return self.blocks.values()
    def neighbors_x_y(self, neighbors_size:Coord) -> Iterable[tuple[Coord, PerceivedBlock]]:
        return {c: b for c, b in self.iter if c.x + self.coord.x <= neighbors_size.x and c.y + self.coord.y <= neighbors_size.y}.items()
    def neighbors_x_y_blocks(self, neighbors_size:Coord) -> Iterable[PerceivedBlock]:
        return {c: b for c, b in self.iter if c.x + self.coord.x <= neighbors_size.x and c.y + self.coord.y <= neighbors_size.y}.values()
    
    @property
    def neighbors_9(self) -> Iterable[tuple[Coord, PerceivedBlock]]:
        return self.neighbors_x_y(Coord(3, 3))
    @property
    def neighbors_9_blocks(self) -> Iterable[PerceivedBlock]:
        return self.neighbors_x_y_blocks(Coord(3, 3))
    @property
    def neighbors_4_require(self) -> Iterable[tuple[Coord, PerceivedBlock | None]]:
        coords = self.coord.four_movements()
        return {c: self.require(c) for c in coords}.items()
    @property
    def neighbors_4_require_blocks(self) -> Iterable[PerceivedBlock | None]:
        coords = self.coord.four_movements()
        return {self.require(c) for c in coords}
    @property
    def creature_block(self) -> PerceivedBlock:
        return self.blocks[self.coord]

    def get(self, coord:Coord) -> PerceivedBlock:
        if coord not in self.blocks:
            raise CoordinateNotFoundError('Coord {} was not found'.format(coord))
    
        return self.blocks[coord]
    def require(self, coord:Coord) -> PerceivedBlock | None:
        if coord not in self.blocks:
            return None
        return self.blocks[coord]

class Analysis:
    @staticmethod
    def find_predicate(perception:Perception, predicate:Callable[[PerceivedBlock], bool]) -> set[Coord]:
        return {c for c, b in perception.iter if predicate(b)}

    @staticmethod
    def empty_spaces(perception:Perception) -> set[Coord]:
        return Analysis.find_predicate(perception, predicate=lambda x: not x.has_entity)
    @staticmethod
    def corpses(perception:Perception) -> set[Coord]:
        return Analysis.find_predicate(perception, lambda x: x.get_entity_type() == EntityTypes.CORPSE)
    @staticmethod
    def same_species(perception:Perception, predicate:Callable[[PerceivedBlock], bool] = lambda x: True) -> set[Coord]:
        return Analysis.find_predicate(perception, predicate=lambda x: x.get_entity_type() == EntityTypes.ENTITY and x.entity.specie_id == perception.creature.specie_id) # type: ignore
    @staticmethod
    def other_species(perception:Perception) -> set[Coord]:
        return Analysis.find_predicate(perception, predicate=lambda x: x.get_entity_type() == EntityTypes.ENTITY and x.entity.specie_id != perception.creature.specie_id) # type: ignore
    @staticmethod
    def near_coord(coords:Iterable[Coord], coord_creature:Coord) -> Coord:    
        return min([c for c in coords], key=lambda x: coord_creature.distance_to_other(x))


class Perceiver:
    @staticmethod
    def perceive_entity(creature:Creature) -> PerceivedCreature:
        return PerceivedCreature(
            creature.energy,
            creature.id,
            creature.genome.core.id,
            creature.gender,
            creature.reproductively_capable and Gender.other_sex(creature.gender) == creature.gender,
            creature.physical_ratio
        )
    @staticmethod
    def perceive_corpse(corpse:Corpse) -> PerceivedCreature:
        return PerceivedCreature(
            corpse.energy,
            corpse.id,
            None,
            None,
            False,
            0
        )
    @staticmethod
    def perceive_cell(cell:Cell) -> PerceivedCell:
        is_edible = cell.property_is_in(Properties.EDIBLE)

        food: None | Energy = None
        if is_edible:
            food = cell.get_component(FoodState).food # type: ignore
        
        is_moveble = True if len(cell.required_capabilities) > 0 else False

        moviment_cost = None
        if is_moveble:
            moviment_cost = cell.get_component(MovimentCost).moviment_cost # type: ignore
        
        is_dangerous = cell.property_is_in(Properties.DANGEROUS)
        damage = None
        if is_dangerous:
            damage = cell.get_component(Damage).damage # type: ignore
        
        return PerceivedCell(
            is_moveble,
            is_edible,
            cell.property_is_in(Properties.DANGEROUS),
            moviment_cost,
            food,
            cell.required_capabilities,
            damage
        )
            
def perceive(creature:Creature, territory:Territory, entity_map:EntityMap, coord_creature:Coord, 
             entitys:EntitysRegistry) -> Perception:
    vision_radius = creature.genome.core.vision_radius

    creature_observer = ObserverCreature(
        creature.energy.ratio,
        creature.life.ratio,
        creature.id,
        creature.genome.core.id,
    )
    data = Geometry.neighbors_x_y(coord_creature, territory, entity_map, vision_radius, True)

    perceived:dict[Coord, PerceivedBlock] = {}

    for c, b in data.items():
        perceived_cell = Perceiver.perceive_cell(b.cell)

        id = b.id_creature

        perceived_creature = None
        
        if id is None:
            pass
        elif id.id is EntityTypes.CREATURE:
            perceived_creature = Perceiver.perceive_entity(entitys.get_creature(id))
        elif id.id is EntityTypes.CORPSE:
            perceived_creature = Perceiver.perceive_corpse(entitys.get_corpse(id))
        

        perceived[c] = PerceivedBlock(perceived_cell, perceived_creature)

    return Perception(perceived, creature_observer, coord_creature)

        
