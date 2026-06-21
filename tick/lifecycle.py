from organism.creatures import Creature, EntitiesRegistry, PregnantUterus, CreatureFactory, Corpse
from decisions.perception import Perception, perceive
from random import choice
from systems.reproduction import ReproductiveSystem, UterusSystem, BornData
from systems.physics import SpatialSystem
from decisions.intent import IntentResolver
from core.coord import Coord
from map.world import World, WorldMotor
from map.map import EntityMap
from decisions.presets import PresetExecutor
from systems.death_system import DeathSystem

def born_data_to_creature(born_data:BornData, position:Coord) -> Creature:
    return CreatureFactory.gen_creature(
        position=position,
        creature_type=born_data.genome.core.id,
        genome=born_data.genome,
        initial_energy=born_data.initial_energy
    )

def resolve_death(creature:Creature, world:World) -> None:
    corpse = DeathSystem.generate_corpse(creature)
    WorldMotor.delete_entity(world.entity_map, creature.position,creature.id, world.entities)
    WorldMotor.add_entity(world.territory, world.entity_map, corpse, world.entities)


class RunnerCreature:
    @staticmethod
    def run_basic_fisiology(creature:Creature) -> None:
        creature.energy.sub(creature.basal_metabolism)
        creature.age.add(1)
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            creature.energy.sub(creature.uterus.pregnancy_cost)
    

    @staticmethod
    def run_uterus(creature:Creature, perception:Perception) -> Creature | None:
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            UterusSystem.pass_time(creature)

            if creature.uterus.gestation.is_ready_to_born:
                four = perception.neighbors_4_require
                possibilities = [c for c, b in four if b is not None and SpatialSystem.can_move(b, creature.genome.core.capabilities)]

                if len(possibilities) > 0:
                    new_coord = choice(possibilities)
                    born_data = ReproductiveSystem.to_birth(creature)
                    if born_data is not None:
                        new_child = born_data_to_creature(born_data, new_coord)
                        return new_child

    
    @staticmethod
    def run_intent(creature:Creature, perception:Perception, world:World) -> None:
        preset = IntentResolver.resolve_intent(creature, world.reproductive_buffer, perception)
        if preset is not None:
            PresetExecutor.execute_preset(preset, creature, world, perception)
    @staticmethod
    def run_creature(creature:Creature, world:World) -> None:
        # ALIAS
        entity_map = world.entity_map
        territory = world.territory
        entities = world.entities
        # CODE
        is_dead = DeathSystem.is_dead(creature)
        if is_dead:
            resolve_death(creature, world)
            return None

        RunnerCreature.run_basic_fisiology(creature)
        perception = perceive(
            creature,
            territory,
            entity_map,
            creature.position,
            entities
        )
        new_child = RunnerCreature.run_uterus(creature, perception)
        if new_child is not None:
            WorldMotor.add_entity(territory, entity_map, new_child, entities)
        RunnerCreature.run_intent(creature, perception, world)
        
class RunnerCorpse:
    @staticmethod
    def to_degrade_corpse(corpse:Corpse) -> None:
        corpse.energy.mul(0.95 - corpse.decomposition_time.value/100)
        corpse.decomposition_time.add(1)
    @staticmethod
    def run_corpse(corpse:Corpse, coord_corpse:Coord, entity_map:EntityMap, entities:EntitiesRegistry) -> None:
        if corpse.ready_to_disapear:
            WorldMotor.delete_entity(entity_map, coord_corpse, corpse.id, entities)
        RunnerCorpse.to_degrade_corpse(corpse)
