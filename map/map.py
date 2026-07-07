from __future__ import annotations
from map.cell import Cell, TerrainTypes, gen_cell
from core.coord import Coord
from dataclasses import dataclass
from core.error import CoordinateAlreadyExistsError, CoordinateNotFoundError
from random import sample, randint
from organism.identity import Id
from core.basestorage import BaseStorage
from noise import pnoise2
from enum import Enum

class ScaleGenValues(Enum):
    LONG_LONG = 0.05
    LONG = 0.15
    MEDIUM = 0.30
    FRAGMENTED = 0.60

class Territory(BaseStorage[Coord, Cell]):
    def __init__(self, size_x:int, size_y:int):
        super().__init__()
        self.size_x = size_x
        self.size_y = size_y


    def _already_exists_error(self, key: Coord) -> None:
        raise CoordinateAlreadyExistsError('Coord {} already exists'.format(key))
    def _not_found_error(self, key: Coord) -> None:
        raise CoordinateNotFoundError('Coord {} was not found'.format(key))
    

class EntityMap(BaseStorage[Coord, Id]):
    def _already_exists_error(self, key: Coord) -> None:
        raise CoordinateAlreadyExistsError('Coord {} already exists'.format(key))
    def _not_found_error(self, key: Coord) -> None:
        raise CoordinateNotFoundError('Coord {} was not found'.format(key))
@dataclass(frozen=True)
class LigateCellView:
    coord:Coord
    cell:Cell

@dataclass(frozen=True)
class BlockData:
    cell:Cell
    id_creature: Id | None



class TerrainQuery:
    @staticmethod
    def all_unucoppied_coords(entity_map:EntityMap, territory:Territory) -> set[Coord]:
        return {c for c in territory.keys if not entity_map.exists(c)}
    @staticmethod
    def random_free_coord(territory:Territory, entity_map:EntityMap, n:int) -> list[Coord]:
        return sample([c for c in territory.keys if not entity_map.exists(c)], n)
    
    @staticmethod
    def get_coord_by_id(id:Id, entity_map:EntityMap) -> Coord:
        return entity_map.get_key_by_value(id) # <- O(n)
    
    



class TerrainMotor:
    @staticmethod
    def add_entity(id:Id, coord:Coord, territory:Territory, entity_map:EntityMap) -> None:
        if not territory.exists(coord):
            raise CoordinateNotFoundError('Coord {} was not found'.format(coord))
        entity_map.add(coord, id)

    @staticmethod
    def delete_entity_by_id(id:Id, entity_map:EntityMap) -> None:
        coord = entity_map.get_key_by_value(id) # <- O(n)
        entity_map.delete(coord)
        # O(n)


    @staticmethod
    def delete_coord(coord:Coord, territory:Territory, entity_map:EntityMap) -> None:
        territory.delete(coord)
        if entity_map.exists(coord):
            entity_map.delete(coord)
        
    @staticmethod
    def move(old_coord:Coord, new_coord:Coord, entity_map:EntityMap, territory:Territory) -> None:
        if not territory.exists(new_coord):
            raise CoordinateNotFoundError('Coord {} was not found in territory'.format(new_coord))
        id = entity_map.get(old_coord)
        entity_map.delete(old_coord)
        entity_map.add(new_coord, id)




class Geometry:
    @staticmethod
    def neighbors_x_y(coord:Coord, territory:Territory, entity_map:EntityMap, radius:Coord, include_self:bool) -> dict[Coord, BlockData]:
        neighbors_dict:dict[Coord, BlockData] = {}
        x = radius.x
        y = radius.y


        for row in range(-y, y+1):
            for column in range(-x, x+1):
                if row == 0 and column == 0:
                    if include_self:
                        neighbors_dict[coord] = BlockData(territory.get(coord), None)
                    continue


                coord_moved = Coord(y=row + coord.y, x=column + coord.x)

                if territory.exists(coord_moved):
                    creature_id = entity_map.require(coord_moved)
                    cell = territory.get(coord_moved)
                    data = BlockData(cell, creature_id)

                    neighbors_dict[coord_moved] = data
        
        return neighbors_dict

class TerrainFactory:
    @staticmethod
    def gen_seed() -> int:
        return randint(0, 10000)
    
    @staticmethod
    def value_to_cell_type(value:float) -> TerrainTypes:
        if value <= -0.3:
            return TerrainTypes.WATER
        elif value <= -0.1:
            return TerrainTypes.SAND
        elif value <= 0.7:
            return TerrainTypes.DIRT
        else:
            return TerrainTypes.ROCK
    
    @staticmethod
    def gen_terrain(radius:Coord, scale:float, seed:int) -> Territory:
        territory = Territory(radius.x, radius.y)

        for x in range(radius.x):
            for y in range(radius.y):
                value:float = pnoise2(x * scale, y * scale, base=seed) # type: ignore
                assert isinstance(value, float)
                terrain_type = TerrainFactory.value_to_cell_type(value)
                cell = gen_cell(terrain_type)
                territory.add(Coord(x, y), cell)
        return territory
    