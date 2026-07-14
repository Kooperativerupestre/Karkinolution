from __future__ import annotations

from dataclasses import dataclass, InitVar, field
from enum import Enum, auto
from random import choice, uniform
from types import MappingProxyType

from karkinolution.organism.identity import Id

@dataclass
class AttackedEvent:
    attacker_id:Id
    damage:int | float

class Gender(Enum):
    MALE = auto()
    FEMALE = auto()

    @staticmethod
    def choice() -> Gender:
        return choice([Gender.MALE, Gender.FEMALE])
    @staticmethod
    def other_sex(gender:Gender) -> Gender:
        if gender is Gender.MALE:
            return Gender.FEMALE
        elif gender is Gender.FEMALE:
            return Gender.MALE

class FoodHint(Enum):
    CORPSE = auto()
    TARGET = auto()
    GRASS = auto()



@dataclass(frozen=True)
class Diet:
    corpse_score:InitVar[float]
    grass_score:InitVar[float]
    target_score:InitVar[float]

    scores:dict[FoodHint, float] = field(default_factory=dict)

    def __post_init__(self, corpse_score:float, grass_score:float, target_score:float):
        max_score = max([corpse_score, grass_score, target_score])
        corpse_score/=max_score
        grass_score/=max_score
        target_score/=max_score
        d = {
            FoodHint.CORPSE: corpse_score,
            FoodHint.GRASS: grass_score,
            FoodHint.TARGET: target_score
        }
        proxy = MappingProxyType(d)
        object.__setattr__(self, 'scores', proxy)
    def __getitem__(self, food_hint:FoodHint) -> float:
        return self.scores[food_hint]
    

    def scramble(self, other_diet:Diet) -> Diet:
        def mini_scramble(v1:int | float, v2:int | float) -> int | float:
            return (v1 + v2)/2 * uniform(0.90, 1.10)
        
        return Diet(
            mini_scramble(self[FoodHint.CORPSE], other_diet[FoodHint.CORPSE]),
            mini_scramble(self[FoodHint.GRASS], other_diet[FoodHint.GRASS]),
            mini_scramble(self[FoodHint.TARGET], other_diet[FoodHint.TARGET])
        )

    

class Temperament(Enum):
    PASSIVE = auto()
    NEUTRAL = auto()
    AGGRESSIVE = auto()
    TERRITORIAL = auto()

