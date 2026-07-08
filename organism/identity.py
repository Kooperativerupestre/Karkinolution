from enum import Enum, auto
from dataclasses import dataclass
from uuid import uuid4

class EntityTypes(Enum):
    CREATURE = auto()
    CORPSE = auto()

@dataclass(frozen=True)
class Id:
    id:str
    e_type:EntityTypes

def gen_id() -> str:
    return str(uuid4().hex)[16:]

def create_creature_id(id:str) -> Id:
    return Id(id, EntityTypes.CREATURE)
def create_corpse_id(id:str) -> Id:
    return Id(id, EntityTypes.CORPSE)
