from __future__ import annotations

from dataclasses import dataclass
from enum import Enum, auto
from random import choice, uniform

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
    corpse_score:float
    grass_score:float
    target_score:float

    def scramble(self, other_diet:Diet) -> Diet:
        def mini_scramble(v1:int | float, v2:int | float) -> int | float:
            return (v1 + v2)/2 * uniform(0.90, 1.10)
        
        return Diet(
            mini_scramble(self.corpse_score, other_diet.corpse_score),
            mini_scramble(self.grass_score, other_diet.grass_score),
            mini_scramble(self.target_score, other_diet.target_score)
        )


    

class Temperament(Enum):
    PASSIVE = auto()
    NEUTRAL = auto()
    AGGRESSIVE = auto()
    TERRITORIAL = auto()

