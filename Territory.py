from __future__ import annotations
from Stats import Energy
from dataclasses import dataclass, field
from random import sample, choice
from uuid import uuid4

from Crabs import Crab, CrabsRegistry, Actions
from CellCoord import Cell, Coord


def pr():
    print('-'*30)

class Territory:
    '''
    (Coord - Cell) & (Coord - ID) vão ser separados em duas classe, ao invés de só uma como a atual
    '''
    def __init__(self):
        self.territory:dict[Coord, Cell] = {}
        self.grid:dict[Coord, str] = {}
        self.positions:dict[str, Coord] = {}

    @property
    def list_id(self) -> set[str]:
        return set(self.grid.values())
    @property
    def list_cells(self) -> list[Cell]:
        return list(self.territory.values())


    def exists_coord(self, coord:Coord) -> bool:
        return coord in self.territory
    def occupied(self, coord:Coord) -> bool:
        return coord in self.grid
    def exists_id(self, id:str) -> bool:
        return id in self.positions
            
    def add(self, id:str, coord:Coord) -> None:
        if not self.exists_coord(coord):
            raise ValueError('Coord {} does not exists'.format(coord))
        elif self.occupied(coord):
            raise ValueError('Coord (x: {} is occupied'.format(coord))

        self.grid[coord] = id
        self.positions[id] = coord
    
    def delete(self, id:str) -> None:
        if id not in self.positions:
            raise ValueError('ID {} does not exists'.format(id))

        coord = self.positions[id]
        del self.positions[id]
        del self.grid[coord]

    def move(self, id:str, new_coord:Coord) -> None:
        self.delete(id)
        self.add(id, new_coord)

    def get_coord(self, id:str) -> Coord:
        if not self.exists_id(id):
            raise ValueError('ID {} does not exists in territory'.format(id))
        
        return self.positions[id]
    def get_id(self, coord:Coord) -> str:
        if not self.exists_coord(coord):
            raise ValueError('Coord {} does not exists'.format(coord))
        elif not self.occupied(coord):
            raise ValueError('Coord {} exists but is not occupied'.format(coord))
        return self.grid[coord]
    
    def get_cell_by_coord(self, coord:Coord) -> Cell:
        if not self.exists_coord(coord):
            raise ValueError('Coord {} does not exists'.format(coord))
        return self.territory[coord]
    def get_cell_by_id(self, id:str) -> Cell:
        '''
        Função de convenção de acesso rápido, atalho
        '''
        
        if not self.exists_id(id):
            raise ValueError('ID {} does not exists in territory'.format(id))
        coord =  self.positions[id]
        return self.territory[coord]
    
    
    def generate(self, y:int, x:int) -> None:
        '''
        Função rápida
        '''
        for y in range(y):
            for x in range(x):
                self.territory[Coord(x=x, y=y)] = Cell()




class Geometry:
    @staticmethod
    def neighbors_4(coord:Coord, territory:Territory) -> dict[Coord, Cell]:
        neighbors_dict = {}

        left = Coord(x=coord.x - 1, y=coord.y)
        right = Coord(x=coord.x + 1, y=coord.y)
        up = Coord(x=coord.x, y=coord.y + 1)
        down = Coord(x=coord.x, y=coord.y - 1)

        pos_coord = [left, right, up, down]

        for coord in pos_coord:
            if territory.exists_coord(coord):
                neighbors_dict[coord] = territory.get_cell_by_coord(coord)
        return neighbors_dict 
    @staticmethod
    def neighbors_x_y(coord:Coord, territory:Territory, x:int, y:int) -> dict[Coord, Cell]:
        '''
        Nota: Devo implementar mais formas de gerenciar espaço, de coletar espaço
        Entre outros, por enquanto, como não tem exigência geométrica ainda, não serão adicionados
        Mas devem. Funções como metódo de calcular distância, entre outros.
        '''
        neighbors_dict = {}


        for row in range(-y, y+1):
            for column in range(-x, x+1):
                if row == 0 and column == 0:
                    continue
                coord_moved = Coord(y=row + coord.y, x=column + coord.x)
                if territory.exists_coord(coord_moved):
                    neighbors_dict[coord_moved] = territory.get_cell_by_coord(coord_moved)
        
        return neighbors_dict
