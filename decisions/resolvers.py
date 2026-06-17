from organism.creatures import Creature, EntitiesRegistry
from decisions.presets import ReproducePreset
from random import choice
from systems.reproduction import Parents, ReproductiveSystem
from decisions.perception import Perception
from organism.identity import Id

class ReproductiveResolver:
    @staticmethod
    def find_adjacent_mates(creature:Creature, perception:Perception) -> list[Id]:
        ids:list[Id] = []


        neighbors = perception.neighbors_4_require_blocks

        for b in neighbors:
            if b is not None and b.has_entity and b.entity.can_reproduce: # type: ignore
                assert b.entity is not None
                ids.append(b.entity.identity) 
        return ids
    @staticmethod
    def resolve_parents(A:Creature, B_id:Id, entities:EntitiesRegistry) -> Parents:
        B = entities.get_creature(B_id)


        return ReproductiveSystem.return_parents(A, B)
    
    @staticmethod
    def chose_mate(ids:list[Id]) -> Id:
        return choice(ids)
    
    
    @staticmethod
    def mate_to_preset(parents:Parents) -> ReproducePreset:
        return ReproducePreset(parents.female, parents.male)
    
    @staticmethod
    def resolve_reproduction(creature:Creature, perception:Perception, entities:EntitiesRegistry) -> ReproducePreset | None:
        mates = ReproductiveResolver.find_adjacent_mates(creature, perception)
        if len(mates) == 0:
            return None
        mate = ReproductiveResolver.chose_mate(mates)
        parents = ReproductiveResolver.resolve_parents(creature, mate, entities)
        return ReproductiveResolver.mate_to_preset(parents)
    



