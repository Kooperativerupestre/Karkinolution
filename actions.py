from enum import Enum, auto
from dataclasses import dataclass
from stats import Age

class Actions(Enum):
    EAT = auto()
    REPRODUCE = auto()
    ATACK = auto()
    NOTHING = auto()
    BORN = auto()

class MoveActions(Enum):
    WALK = auto()
    SWIMM = auto()
    MOVE = auto()

class IntentActs(Enum):
    FIND_FOOD = auto()
    FIND_MATCH = auto()
    NOTHING = auto()

class Intent:
    def __init__(self, intent:IntentActs):
        self.intent = intent
        self.time = 0





