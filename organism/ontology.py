from enum import Enum, auto
from dataclasses import dataclass
from random import choice
from organism.identity import Id

@dataclass
class AtackedEvent:
    atacker_id:Id
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

class Diet(Enum):
    CARNIVORE = auto()
    HERBIVORE = auto()
    OMNIVORE = auto()    


class Temperament(Enum):
    PASSIVE = auto()
    NEUTRAL = auto()
    AGGRESSIVE = auto()
    TERRITORIAL = auto()

