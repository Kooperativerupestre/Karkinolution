from __future__ import annotations
from dataclasses import dataclass
from math import sqrt


@dataclass(frozen=True)
class Coord:
    x:int
    y:int

    def distance_to_other(self, other:Coord) -> int | float:
        return sqrt((other.x - self.x)**2 + (other.y - self.y)**2)
 
    def distance_exceeds_one(self, other:Coord) -> bool:
        return self.distance_to_other(other) > 1
    def four_movements(self) -> set[Coord]:
        return {Coord(self.x+1, self.y), Coord(self.x, self.y + 1), Coord(self.x - 1, self.y), Coord(self.x, self.y - 1)}
    def sub(self, other:Coord) -> Coord:
        return Coord(x=self.x - other.x, y=self.y - other.y)
    def add(self, other:Coord) -> Coord:
        return Coord(x=self.x + other.x, y=self.y + other.y)
    def div(self, div_number:int) -> Coord:
        return Coord(x=round(self.x/div_number), y=round(self.y/div_number))
    def mul(self, mul_number:int) -> Coord:  
        return Coord(x=self.x*mul_number, y=self.y*mul_number)  
    def __str__(self):
        return f'x: {self.x}, y: {self.y}'
