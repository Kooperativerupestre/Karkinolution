from Stats import Energy
from dataclasses import dataclass


class Cell:
    def __init__(self):
        self.energy = Energy(5, 10)
        self.regen_tax = 10
    @property
    def pretty(self) -> str:
        return  '# {}'.format(self.energy.value)
    def __str__(self):
        return f'Cell({self.energy.value})'
    def regen(self) -> None:
        self.energy.add(self.regen_tax)

@dataclass(frozen=True)
class Coord:
    x:int
    y:int
    def __str__(self):
        return f'(x: {self.x}, y: {self.y})'

