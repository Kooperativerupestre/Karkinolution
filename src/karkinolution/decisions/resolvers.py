from random import choice

from karkinolution.core.coord import Coord
from karkinolution.terrain.world import World, LogEntry

from karkinolution.decisions.perception import (
    Perception,
    PerceptionAnalyser,
    PerceivedCreature
) 
from karkinolution.decisions.presets import ReproducePreset

from karkinolution.organism.creatures import (
    Creature,
    EntitiesRegistry,
    CreatureFactory
)
from karkinolution.organism.identity import Id

from karkinolution.systems.reproduction import (
    Parents,
    ReproductiveSystem,
    BornData
)

class ReproductiveResolver:
    @staticmethod
    def find_adjacent_mates(creature:Creature, perception:Perception) -> list[Id]:
        ids:list[Id] = []


        neighbors = PerceptionAnalyser.neighbors_8(perception)


        for b in neighbors.blocks:
            if b.has_creature and ReproductiveSystem.can_reproduce(creature, b.entity):
                assert isinstance(b.entity, PerceivedCreature)
                ids.append(b.entity.id)
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
    

class BornResolver:
    @staticmethod
    def born_data_to_creature(born_data:BornData, position:Coord) -> Creature:
        return CreatureFactory.gen_creature(
            position=position,
            creature_type=born_data.genome.core.id,
            genome=born_data.genome,
            initial_energy=born_data.initial_energy,
            sociability=born_data.sociability
        )
    @staticmethod
    def resolve_born_data(born_data:BornData | None, position:Coord, world:World, mother_name:str) -> Creature | None:
        if born_data is None: # died: yes
            world.log.add(LogEntry(world.time, "one child of creature {} has died".format(mother_name)))
            return None
        # died: no
        new_child = BornResolver.born_data_to_creature(born_data, position)
        world.log.add(LogEntry(world.time, "creature {} child of {}, was born".format(new_child.name, mother_name)))
        return new_child
           