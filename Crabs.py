from Stats import Energy
from uuid import uuid4
from random import choice
from enum import Enum, auto
from CellCoord import Cell, Coord
from dataclasses import dataclass


class Actions(Enum):
    EAT = auto()
    MOVE = auto()
    NOTHING = auto()


def gen_id() -> str:
    return str(uuid4().hex)[16:]

@dataclass
class ViewCrab:
    hungry:float
    is_dead:bool

class Crab:
    def __init__(self):
        self.energy = Energy(5, 5)
        self.id = gen_id()
    
    @property
    def energy_needed(self) -> float:
        return self.energy.limit - self.energy.value
    @property
    def hungry(self) -> float:
        return 1 - self.energy.value / self.energy.limit
    @property
    def is_dead(self) -> bool:
        return self.energy.value <= 0
    def __str__(self):
        return 'Crab: ID: {}, ENERGY {}'.format(self.id, self.energy.value)
    @property
    def view(self) -> ViewCrab:
        return ViewCrab(
            hungry=self.hungry,
            is_dead=self.is_dead
        )


class Instincts:
    @staticmethod
    def need_eat(crab:Crab) -> bool:
        return crab.hungry > 0.65
    @staticmethod
    def need_move(crab:Crab) -> bool:
        return choice([True, False])
    @staticmethod
    def next_actions(crab:Crab) -> list[Actions]:
        actions = []

        if Instincts.need_eat(crab):
            actions.append(Actions.EAT)
        if Instincts.need_move(crab):
            actions.append(Actions.MOVE)
        # Transformar depois isso em um for, onde passa por essas funções aplicando e adicionando-as
        # Para substituir if
        

        if len(actions) == 0:                   
            actions.append(Actions.NOTHING)
        return actions

class CrabsRegistry:
    def __init__(self):
        self._crabs = {}
    
    @property
    def list_id(self) -> set[str]:
        return set(self._crabs.keys())
    @property
    def list_crabs(self) -> set[Crab]:
        return set(self._crabs.values())

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
    
