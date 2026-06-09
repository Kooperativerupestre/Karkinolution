from __future__ import annotations
from core.cell import Cell
from core.coord import Coord
from dataclasses import dataclass
from core.error import (CoordinateNotFoundError, IdNotFoundError, CoordinateExistenceError, IdExistenceError,
                   CoordinateOccupiedError)
from typing import Iterable
from random import sample
from organism.identity import Id

class Territory:
    def __init__(self):
        self.territory:dict[Coord, Cell] = {}
    @property
    def coords(self) -> Iterable[Coord]:
        return self.territory.keys()
    @property
    def cells(self) -> Iterable[Cell]:
        return self.territory.values()

class EntityMap:
    def __init__(self):
        self.entity_map:dict[Coord, Id] = {}
    @property
    def coords(self) -> Iterable[Coord]:
        return self.entity_map.keys()
    @property
    def creatures(self) -> Iterable[Id]:
        return self.entity_map.values()
    @property
    def iter(self) -> Iterable[tuple[Coord, Id]]:
        return self.entity_map.items()
@dataclass(frozen=True)
class LigateCellView:
    coord:Coord
    cell:Cell

@dataclass(frozen=True)
class BlockData:
    cell:Cell
    id_creature: Id | None


    
class TerrainView:
    @staticmethod
    def exists_coord(coord:Coord, terrain:Territory | EntityMap) -> bool:
        if isinstance(terrain, Territory):
            return coord in terrain.territory
        else: # EntityMap
            return coord in terrain.entity_map
    @staticmethod
    def is_occupied(coord:Coord, entity_map:EntityMap) -> bool:
        return coord in entity_map.entity_map
    @staticmethod
    def exists_id(id:Id, entity_map:EntityMap) -> bool:
        return id in entity_map.entity_map.values()
    @staticmethod
    def all_unucoppied_coords(entity_map:EntityMap, territory:Territory) -> set[Coord]:
        return {c for c in territory.coords if not TerrainView.is_occupied(c, entity_map)}
    @staticmethod
    def random_free_coord(territory:Territory, entity_map:EntityMap, n:int) -> list[Coord]:
        return sample([c for c in territory.coords if not TerrainView.is_occupied(c, entity_map)], n)
    
    
    
    @staticmethod
    def get_cell_by_coord(coord:Coord, territory:Territory) -> Cell:
        if not TerrainView.exists_coord(coord, territory):
            raise CoordinateNotFoundError('Coord ({}) was not found'.format(coord))
        # Territory stores cells in territory.territory dict
        return territory.territory[coord]  # type: ignore
    @staticmethod
    def get_id_by_coord(coord:Coord, entity_map:EntityMap) -> Id:
        if not TerrainView.exists_coord(coord, entity_map):
            raise CoordinateNotFoundError(f'Coord ({coord}) was not found')
        return entity_map.entity_map[coord]


    @staticmethod
    def require_id_by_coord(coord:Coord, entity_map:EntityMap) -> Id | None:
        if not TerrainView.exists_coord(coord, entity_map):
            return None
        return entity_map.entity_map[coord]
        

    @staticmethod
    def get_coord_by_id(id:Id, entity_map:EntityMap) -> Coord:
        if not TerrainView.exists_id(id, entity_map):
            raise IdNotFoundError('ID {} was not found'.format(id.id))
        new = {v: k for k, v in entity_map.entity_map.items()}
        return new[id]
    @staticmethod
    def ligate_creature_to_cell(id:Id, entity_map:EntityMap, territory:Territory) -> LigateCellView:
        coord = TerrainView.get_coord_by_id(id, entity_map)
        cell = TerrainView.get_cell_by_coord(coord, territory)

        return LigateCellView(
            coord=coord,
            cell=cell
        )



class TerrainMotor:
    @staticmethod
    def add_entity(id:Id, coord:Coord, territory:Territory, entity_map:EntityMap) -> None:
        if TerrainView.exists_id(id, entity_map):
            raise IdExistenceError('ID {} already exists'.format(id.id))
        if not TerrainView.exists_coord(coord, territory):
            raise CoordinateNotFoundError('Coord ({}) does not exists in territory'.format(coord))
        entity_map.entity_map[coord] = id
    @staticmethod
    def delete_entity(id:Id, entity_map:EntityMap) -> None:
        if not TerrainView.exists_id(id, entity_map):
            raise IdNotFoundError('ID {} was not found'.format(id.id))
        coord = TerrainView.get_coord_by_id(id, entity_map)         # O(n)
        del entity_map.entity_map[coord]

    @staticmethod
    def delete_entity_by_coord(coord:Coord, entity_map:EntityMap) -> None:
        if not TerrainView.exists_coord(coord, entity_map):
            raise CoordinateNotFoundError('Coord ({}) does not exists in territory'.format(coord))
        del entity_map.entity_map[coord]
    @staticmethod
    def add_coord(coord:Coord, cell:Cell, territory:Territory) -> None:
        if TerrainView.exists_coord(coord, territory):
            raise CoordinateExistenceError('Coord ({}) already exists'.format(coord))
        territory.territory[coord] = cell
    @staticmethod
    def delete_coord(coord:Coord, territory:Territory, entity_map:EntityMap) -> None:
        if not TerrainView.exists_coord(coord, territory):
            raise CoordinateNotFoundError('Coord ({}) was not found'.format(coord))
        del territory.territory[coord]
        del entity_map.entity_map[coord]
    @staticmethod
    def move(id:Id, new_coord:Coord, entity_map:EntityMap, territory:Territory) -> None:
        if not TerrainView.exists_coord(new_coord, territory):
            raise CoordinateNotFoundError('Coord ({}) was not found'.format(new_coord))
        if TerrainView.is_occupied(new_coord, entity_map):
            raise CoordinateOccupiedError('Coord ({}) was occupied'.format(new_coord))
        TerrainMotor.delete_entity(id, entity_map)
        TerrainMotor.add_entity(id, new_coord, territory, entity_map)




class Geometry:
    @staticmethod
    def neighbors_x_y(coord:Coord, territory:Territory, entity_map:EntityMap, x:int, y:int, include_self:bool) -> dict[Coord, BlockData]:
        neighbors_dict = {}


        for row in range(-y, y+1):
            for column in range(-x, x+1):
                if row == 0 and column == 0:
                    if include_self:
                        neighbors_dict[coord] = BlockData(TerrainView.get_cell_by_coord(coord, territory), None)
                    continue


                coord_moved = Coord(y=row + coord.y, x=column + coord.x)

                if TerrainView.exists_coord(coord_moved, territory):
                    creature = TerrainView.require_id_by_coord(coord_moved, entity_map)
                    cell = TerrainView.get_cell_by_coord(coord_moved, territory)
                    data = BlockData(cell, creature)
                    neighbors_dict[coord_moved] = data
        
        return neighbors_dict


