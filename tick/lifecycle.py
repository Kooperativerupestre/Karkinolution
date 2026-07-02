from organism.creatures import Creature, EntitiesRegistry, PregnantUterus, CreatureFactory, Corpse
from decisions.perception import Perception, perceive
from random import choice, choices
from systems.reproduction import ReproductiveSystem, UterusSystem, BornData
from systems.physics import MovementSystem
from decisions.intent import IntentResolver
from core.coord import Coord
from map.world import World, WorldMotor, LogEntry
from map.map import EntityMap
from decisions.presets import PresetExecutor, MovePreset, EatPreset, AttackPreset, ReproducePreset
from systems.death_system import DeathSystem
from decisions.resolvers import ReproductiveResolver

def born_data_to_creature(born_data:BornData, position:Coord) -> Creature:
    return CreatureFactory.gen_creature(
        position=position,
        creature_type=born_data.genome.core.id,
        genome=born_data.genome,
        initial_energy=born_data.initial_energy
    )

def resolve_death(creature:Creature, world:World) -> None:
    corpse = DeathSystem.generate_corpse(creature)
    WorldMotor.delete_entity(world.entity_map, creature.id, world.entities)
    world.log.add(LogEntry(world.time, "Creature {} died".format(creature)))
    if corpse.time_left == 0:
        return None
    WorldMotor.add_entity(world, corpse)



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
                possibilities = MovementSystem.four_movable_coords(perception, creature)
                if len(possibilities) > 0:
                    new_coord = choice(possibilities)
                    born_data = ReproductiveSystem.to_birth(creature)
                    if born_data is not None:
                        new_child = born_data_to_creature(born_data, new_coord)
                        return new_child

    @staticmethod
    def get_idle_preset(creature:Creature, perception:Perception, world:World) -> MovePreset | None:
        actions = [True, False] # MOVE OR NO
        weights = [1 - creature.hungry, creature.hungry]

        chosen = choices(actions, weights=weights, k=1)[0]
        if chosen is True:
            four_coords = MovementSystem.four_movable_coords(perception, creature)
            if len(four_coords) == 0:
                return None
            preset = MovePreset(choice(four_coords))
            return preset

    @staticmethod
    def try_to_get_reproduce_preset(creature: Creature, perception:Perception, world:World) -> ReproducePreset | None:
        if creature.id in world.reproductive_buffer.desires:
            return ReproductiveResolver.resolve_reproduction(creature, perception, world.entities)
    @staticmethod
    def get_presets(creature:Creature, perception:Perception, world:World) -> list[MovePreset | EatPreset | AttackPreset | ReproducePreset]:
        presets:list[MovePreset | EatPreset | AttackPreset | ReproducePreset] = []
        preset = IntentResolver.resolve_intent(creature, world, perception)
        if preset is None:
            preset = RunnerCreature.get_idle_preset(creature, perception, world)
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
            creature.position,
            entities
        )
  
        presets = RunnerCreature.get_presets(creature, perception, world)
        new_child = RunnerCreature.run_uterus(creature, perception)



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
