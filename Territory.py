from __future__ import annotations
from Stats import Energy
from dataclasses import dataclass, field
from random import sample, choice
from uuid import uuid4

from Crabs import Crab, CrabsRegistry, Actions


class Cell:
    def __init__(self):
        self.energy = Energy(10, 40)
        self.regen_tax = 10
    @property
    def pretty(self) -> str:
        return  '# {}'.format(self.energy.value)
    def regen(self) -> None:
        self.energy.add(self.regen_tax)

@dataclass(frozen=True)
class Coord:
    x:int
    y:int
    def __str__(self):
        return f'(x: {self.x}, y: {self.y})'



class Motor:
    '''
    Modelo atual usa uma definição pré-territorial.
    Que vai ser mudado assim que step não estar mais em territory.
    Além disso, algumas mecanicas de movimentação e reconhecimento também serão desacopladas.

    Além de lógicas monstruosas como 'move', que estão mal-separadas, serão organizadas em uma classe.

    '''
    ### Connections
    @staticmethod
    def connect(id:str, registry:CrabsRegistry) -> Crab:
        return registry.get(id)

        

    ### ACTIONS OF CRABS
    @staticmethod
    def eat(crab:Crab, cell:Cell) -> None:
        cost = min(cell.energy.value, crab.energy_needed)

        cell.energy.sub(cost)
        crab.energy.add(cost)

    @staticmethod
    def move(crab:Crab, coord:Coord, territory:Territory) -> None:
        territory.delete(crab.id)
        territory.add(crab, coord)
    @staticmethod
    def decide_move(crab:Crab, territory:Territory) -> Coord:
        '''
        Possível possibilidade de não movimentalidade considerada, mas não tratada ainda (total)
        '''
        
        coord_of_crab = territory.get_coord(crab.id)

        neighbors = list(territory.neighbors_4(coord_of_crab).keys())
        neighbors = [n for n in neighbors if not territory.occupied(n)]
        next_cell = choice(neighbors)
        return next_cell
    @staticmethod
    def move_dinamic(crab:Crab, territory:Territory) -> None:
        next_cell =  Motor.decide_move(crab, territory)

        crab.energy.sub(1)

        Motor.move(crab, next_cell, territory)

    @staticmethod
    def interpret_and_do(crab:Crab, territory:Territory) -> None:
        if crab.next_action() == Actions.EAT:
            print("EAT")
            Motor.eat(crab, territory.get_cell_by_id(crab.id))
        elif crab.next_action() == Actions.MOVE:
            Motor.move_dinamic(crab, territory)
        elif crab.next_action() is None:
            pass

    ### STATES
    @staticmethod
    def is_dead(crab:Crab) -> bool:
        return crab.energy.value <= 0
    @staticmethod
    def visualizer(territory:Territory, registry:CrabsRegistry) -> None:
        '''
        (Obviamente funções de visualizar serão chutadas para fora)
        '''

        territory.pretty
        registry.pretty

    @staticmethod
    def add_crab(crab:Crab, territory:Territory, registry:CrabsRegistry, coord:Coord = Coord(0, 0)) -> None:
        territory.add(crab, coord)
        registry.add(crab)

    @staticmethod
    def delete_crab(id:str, territory:Territory, registry:CrabsRegistry) -> None:
        territory.delete(id)
        registry.delete(id)


def pr():
    print('-'*30)

class Territory:
    '''
    Há decisões estruturais aqui viciosas que serão retiradas depois em atualizações futuras.
    O código atual está em protótipo. Coisas que claramente irã mudar:

    Desacoplamento de step(), óbvio, ele não depende só do território
    Ele depende do motor de ações dos crabs, e também do território
    Não podendo estar dentro de nenhum dois

    Funções de coordenadas, também serão jogadas para fora
    Apenas algumas funções de verificações de coordenadas, vão permanecer em territory.

    Além que vai ter mudanças estruturais na forma como territory é distribuido
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
    def pretty(self) -> None:
        print('Territory')
        print('Coord-Cell')
        for coord, cell in self.territory.items():
            print(f'{coord}  -  {cell.pretty}')
        print('-'*30)
        print('Creature-Coord')
        for id, coord in self.positions.items():
            print(f'{id}  -  {coord}')

    def exists_coord(self, coord:Coord) -> bool:
        return coord in self.territory
    def occupied(self, coord:Coord) -> bool:
        return coord in self.grid
    def exists_id(self, id:str) -> bool:
        return id in self.positions
            
    def add(self, creature:Crab, coord:Coord) -> None:
        if not self.exists_coord(coord):
            raise ValueError('Coord {} does not exists'.format(coord))
        elif self.occupied(coord):
            raise ValueError('Coord (x: {} is occupied'.format(coord))

        self.grid[coord] = creature.id
        self.positions[creature.id] = coord
    
    def delete(self, id:str) -> None:
        if id not in self.positions:
            raise ValueError('ID {} does not exists'.format(id))

        coord = self.positions[id]
        del self.positions[id]
        del self.grid[coord]

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
        if not self.exists_id(id):
            raise ValueError('ID {} does not exists in territory'.format(id))
        coord =  self.positions[id]
        return self.territory[coord]
    
    def neighbors_4(self, coord:Coord) -> dict[Coord, Cell]:
        neighbors_dict = {}

        left = Coord(x=coord.x - 1, y=coord.y)
        right = Coord(x=coord.x + 1, y=coord.y)
        up = Coord(x=coord.x, y=coord.y + 1)
        down = Coord(x=coord.x, y=coord.y - 1)

        pos_coord = [left, right, up, down]

        for coord in pos_coord:
            if self.exists_coord(coord):
                neighbors_dict[coord] = self.get_cell_by_coord(coord)
        return neighbors_dict 

    
    
    def generate(self, y:int, x:int) -> None:
        for y in range(y):
            for x in range(x):
                self.territory[Coord(x=x, y=y)] = Cell()



    @staticmethod
    def is_dead(crab:Crab) -> bool:
        return crab.energy.value <= 0
    # Que nojo desse código namoral. Não vejo a hora de fazer o primeiro loop.
    def step(self, registry:CrabsRegistry) -> None:
        self.creatures_id = sample(list(self.list_id), k=len(self.list_id))
        for id in self.creatures_id:
            crab = registry.get(id)
            if Motor.is_dead(crab):
                Motor.delete_crab(id, self, registry)

            crab.energy.sub(1)
            
            Motor.interpret_and_do(crab, self)
    

t = Territory()
r = CrabsRegistry()

t.generate(y=3, x=3)

Motor.visualizer(t, r)

Motor.add_crab(Crab(), territory=t, registry=r)

Motor.visualizer(t, r)

print('One step')

t.step(r)

Motor.visualizer(t, r)

print('Two step')

t.step(r)

Motor.visualizer(t, r)

print('Tree step')

t.step(r)

Motor.visualizer(t, r)

t.step(r)

Motor.visualizer(t, r)