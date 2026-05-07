from Stats import Energy
from uuid import uuid4
from random import choice
from enum import Enum, auto


class Actions(Enum):
    EAT = auto()
    MOVE = auto()


def gen_id() -> str:
    return str(uuid4().hex)[16:]

class Crab:
    def __init__(self):
        self.energy = Energy(5, 5)
        self.id = gen_id()
        
    @property
    def energy_needed(self) -> int | float:
        return self.energy.limit - self.energy.value
    @property
    def hungry(self) -> int | float:
        '''
        x/y

        x/5

        2/5 = 0.5
        1 = 
        '''
        return 1 - self.energy.value / self.energy.limit
    @property
    def need_eat(self) -> bool:
        return self.hungry > 0.65
    @property
    def need_move(self) -> bool:
        return choice([True, False])
    @property
    def pretty(self) -> None:
        print('''
Energy: {}
ID: {}
'''.format(self.energy.value, self.id))
    
    def next_action(self) -> Actions | None:
        print('Vontade de comer: ', self.need_eat)
        if self.need_eat:
            return Actions.EAT
        elif self.need_move:
            return Actions.MOVE

    
class CrabsRegistry:
    def __init__(self):
        self._crabs = {}
    
    @property
    def list_id(self) -> set[str]:
        return set(self._crabs.keys())
    @property
    def list_crabs(self) -> set[Crab]:
        return set(self._crabs.values())
    @property
    def pretty(self) -> None:
        print('Registry')
        for elemento in self.list_crabs:
            elemento.pretty

    def add(self, crab:Crab) -> str:
        if crab.id in self.list_id:
            raise ValueError('ID {} exists'.format(crab.id))
        self._crabs[crab.id] = crab
        return crab.id
    def delete(self, id:str) -> None:
        if id not in self.list_id:
            raise ValueError('ID {} does not exists'.format(id))
        del self._crabs[id]
    def get(self, id:str) -> Crab:
        if id not in self.list_id:
            raise ValueError('ID {} does not exists'.format(id))
        return self._crabs[id]
    
