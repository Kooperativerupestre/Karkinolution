from dataclasses import dataclass
from organism.creatures import Creature
from actions import MoveActions
from coord import Coord
from stats import Energy

@dataclass(frozen=True)
class MovePressets:
    creature:Creature
    coord_creature:Coord
    new_coord:Coord
    move_act:MoveActions
@dataclass(frozen=True)
class ReproducePressets:
    female:Creature
    male:Creature
@dataclass(frozen=True)
class AtackPressets:
    atacker:Creature
    target:Creature
@dataclass
class EatPressets:
    creature:Creature
    energy:Energy
