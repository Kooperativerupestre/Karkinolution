from dataclasses import dataclass
from organism.creatures import Creature
from decisions.actions import MoveActions
from core.coord import Coord
from organism.stats import Energy
from organism.identity import Id

@dataclass(frozen=True)
class MovePreset:
    new_coord:Coord
@dataclass(frozen=True)
class ReproducePreset:
    female:Id
    male:Id
@dataclass(frozen=True)
class AtackPreset:
    target:Id
@dataclass(frozen=True)
class EatPreset:
    energy:Energy
