
from karkinolution.organism.creatures import (
    Creature,
    EntitiesRegistry,
    PregnantUterus,
    Corpse,
)
from karkinolution.decisions.perception import Perception, perceive
from random import choice
from karkinolution.systems.reproduction import (
    ReproductiveSystem,
    UterusSystem,
)
from karkinolution.systems.physics import MovementSystem
from karkinolution.decisions.intent import IntentResolver
from karkinolution.terrain.world import World, WorldMotor, LogEntry
from karkinolution.terrain.map import EntityMap
from karkinolution.decisions.presets import (
    PresetExecutor,
    MovePreset,
    EatPreset,
    AttackPreset,
    ReproducePreset,
)
from karkinolution.systems.death import DeathSystem
from karkinolution.decisions.resolvers import ReproductiveResolver, BornResolver
from karkinolution.decisions.instincts import PlannerNothing

from karkinolution.utils.k_random import choice_bool


def resolve_death(creature:Creature, world:World) -> None:
    corpse = DeathSystem.generate_corpse(creature)
    WorldMotor.delete_entity(world.entity_map, creature.id, world.entities)
    world.log.add(LogEntry(world.time, "Creature {} died".format(creature)))
    if corpse.time_left == 0:
        return None
    WorldMotor.add_entity(world, corpse)



def resolve_born(creature:Creature, perception:Perception, world:World) -> Creature | None:
    assert isinstance(creature.uterus, PregnantUterus)
    possibilities = MovementSystem.four_movable_coords(perception, creature)

    if len(possibilities) == 0:
        r = choice_bool(yes_weight=creature.uterus.gravity, no_weight= 1 - creature.uterus.gravity)

        if r:
            born_data = ReproductiveSystem.to_birth(creature)
            possibilities = list(perception.neighbors_8_coords)
            if len(possibilities) == 0:
                return None
            
            new_coord = choice(possibilities)
            return BornResolver.resolve_born_data(born_data, new_coord, world, creature.name)
        return None
    
    born_data = ReproductiveSystem.to_birth(creature)
    new_coord = choice(possibilities)
    return BornResolver.resolve_born_data(born_data, new_coord, world, creature.name)



class RunnerCreature:
    @staticmethod
    def run_basic_fisiology(creature:Creature) -> None:
        creature.energy.sub(creature.basal_metabolism)
        creature.age.add(1)
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            creature.energy.sub(creature.uterus.pregnancy_cost)
    

    @staticmethod
    def run_uterus(creature:Creature, perception:Perception, world:World) -> Creature | None:
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            UterusSystem.pass_time(creature)

            if creature.uterus.gestation.is_ready_to_born:
                return resolve_born(creature, perception, world)
            
    @staticmethod
    def try_to_get_reproduce_preset(creature: Creature, perception:Perception, world:World) -> ReproducePreset | None:
        if creature.id in world.reproductive_buffer.desires:
            return ReproductiveResolver.resolve_reproduction(creature, perception, world.entities)
    @staticmethod
    def get_presets(creature:Creature, perception:Perception, world:World) -> list[MovePreset | EatPreset | AttackPreset | ReproducePreset]:
        presets:list[MovePreset | EatPreset | AttackPreset | ReproducePreset] = []
        preset = IntentResolver.resolve_intent(creature, world, perception)
        if preset is None:
            preset = PlannerNothing.plan_intent(perception, creature)
        if preset is not None:
            presets.append(preset)
        
        preset = RunnerCreature.try_to_get_reproduce_preset(creature, perception, world)
        if preset is not None:
            presets.append(preset)
        return presets
    
    @staticmethod
    def run_creature(creature:Creature, world:World) -> None:
        # ALIAS
        entity_map = world.entity_map
        territory = world.territory
        entities = world.entities
        # CODE
        new_child:Creature | None = None
        is_dead = DeathSystem.is_dead(creature)
        if is_dead:
            resolve_death(creature, world)
            return None

        RunnerCreature.run_basic_fisiology(creature)
        perception = perceive(
            creature,
            territory,
            entity_map,
            entities
        )
  
        presets = RunnerCreature.get_presets(creature, perception, world)
        new_child = RunnerCreature.run_uterus(creature, perception, world)



        if new_child is not None:
            WorldMotor.add_entity(world, new_child)
            for preset in presets:
                if not isinstance(preset, MovePreset):
                    PresetExecutor.execute_preset(preset, creature, world, perception)
        else:
            for preset in presets:
                PresetExecutor.execute_preset(preset, creature, world, perception)

        
class RunnerCorpse:
    @staticmethod
    def to_degrade_corpse(corpse:Corpse) -> None:
        corpse.energy.mul(0.95 - corpse.decomposition_time.value/100)
        corpse.decomposition_time.add(1)
    @staticmethod
    def run_corpse(corpse:Corpse, entity_map:EntityMap, entities:EntitiesRegistry) -> None:
        if corpse.ready_to_disapear:
            WorldMotor.delete_entity(entity_map, corpse.id, entities)
        RunnerCorpse.to_degrade_corpse(corpse)
