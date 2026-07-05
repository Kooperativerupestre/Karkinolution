from __future__ import annotations
from core.error import InsufficientEnergyError


class LimitedValue:
    def __init__(self, value:int | float, limit: int | float, floor:int | float = 0):
        self.value = value
        self.limit = limit
        self.floor = floor
    @property
    def ratio(self) -> int | float:
        '''
        Always return a value in [0, 1]
        '''
        return self.value/self.limit
    def add(self, value:int | float) -> None:
        self.value = min(self.limit, self.value + value)
    def sub(self, value:int | float) -> None:
        self.value = max(self.floor, self.value - value)
    def set(self, value:int | float) -> None:
        self.value = min(max(self.floor, value), self.limit)
    def mul(self, value:int | float) -> None:
        self.set(self.value * value)

class Energy(LimitedValue):
    pass
class Life(LimitedValue):
    pass
class Fertility(LimitedValue):
    def regen(self) -> None:
        self.add(1)
    def zero(self) -> None:
        self.set(0)
    def reproductive_capability(self) -> bool:
        return self.value == self.limit

class Age(LimitedValue):
    def pass_time(self) -> None:
        self.add(1)

def check_energy(energy:Energy, cost:int | float) -> None:
    if energy.value < cost:
        raise InsufficientEnergyError('Energy value {} is insufficient for {}'.format(energy.value, cost))