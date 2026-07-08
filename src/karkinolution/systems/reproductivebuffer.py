from dataclasses import dataclass

from karkinolution.core.error import IdAlreadyExistsError

from karkinolution.organism.genetics import CreatureTypes
from karkinolution.organism.identity import Id

@dataclass(frozen=True)
class ReproductiveDesire:
    creature_id:Id
    specie_id:CreatureTypes

class ReproductiveBuffer:
    def __init__(self):
        self.desires:dict[Id, ReproductiveDesire] = {}

    def require_first_by_specie(self, specie_id:CreatureTypes) -> None | ReproductiveDesire:
        # O(n)

        for desire in self.desires.values():
            if desire.specie_id == specie_id:
                return desire
    def get(self, id:Id) -> ReproductiveDesire:
        return self.desires[id]
    def require(self, id:Id) -> ReproductiveDesire | None:
        if id not in self.desires:
            return None
        return self.desires[id]
    
    def try_remove(self, id:Id) -> None:
        if id in self.desires:
            del self.desires[id]
    def add(self, desire:ReproductiveDesire) -> None:
        id = desire.creature_id
        if id in self.desires:
            raise IdAlreadyExistsError('ID {} already exists'.format(id.id))
        self.desires[id] = desire
    def registry(self, desire:ReproductiveDesire) -> None:
        if desire.creature_id not in self.desires:
            self.desires[desire.creature_id] = desire