from map.map import Territory, EntityMap, TerrainQuery, TerrainFactory, ScaleGenValues
from map.cell import Cell, FoodState
from core.coord import Coord
from organism.genetics import CreatureTypes
from organism.creatures import Creature, Corpse, EntitiesRegistry, CreatureInterface, CreatureFactory, PregnantUterus
from organism.stats import Energy
from organism.identity import Id, EntityTypes
from map.world import WorldMotor, World
from systems.biology import DeathSystem, MetabolismSystem
from systems.reproduction import ReproductiveSystem, BornData, UterusSystem, Parents
from decisions.perception import perceive, Perception
from systems.presets import AttackPreset, EatPreset, MovePreset, ReproducePreset
from decisions.instincts import ReproductiveBuffer
from systems.physics import MovementSystem, AttackSystem, SpatialSystem
from random import choice
from decisions.intent import IntentResolver


# =========================================================
# INIT WORLD
# =========================================================


def print_creature(creature: CreatureInterface, coord: Coord, cell: Cell) -> None:
    print("CREATURE")
    print(
        f"Name: {creature.name} | "
        f"ID: {creature.id.id} | "
        f"Coord: {coord}"
    )

    print(
        f"Energy: {creature.energy.value}/{creature.energy.limit} | "
        f"Life: {creature.life.value}/{creature.life.limit}"
    )

    print(
        f"Intent: {creature.intent.intent.name} | "
        f"Hungry: {creature.hungry}"
    )

    print(
        f"Cell type: {cell.type}"
    )

    print(
        f"Properties: {cell.properties}"
    )

    print(
        f"Components: {cell.extra_values}"
    )

    if cell.component_is_in(FoodState):
        food: Energy = cell.get_component(FoodState).food  # type: ignore

        print(
            f"Cell energy: {food.value}/{food.limit}"
        )

    print("-" * 30)

def print_corpse(corpse: Corpse, coord: Coord, cell: Cell) -> None:
    print("CORPSE")
    print(
        f"ID: {corpse.id.id} | "
        f"Coord: {coord}"
    )

    print(
        f"Energy: {corpse.energy.value}/{corpse.energy.limit}"
    )

    print(
        f"Cell type: {cell.type}"
    )

    print(
        f"Properties: {cell.properties}"
    )

    print(
        f"Components: {cell.extra_values}"
    )

    if cell.component_is_in(FoodState):
        food: Energy = cell.get_component(FoodState).food  # type: ignore

        print(
            f"Cell energy: {food.value}/{food.limit}"
        )

    print("-" * 30)


class Init:



    @staticmethod
    def create_creatures(
        entity_map: EntityMap,
        territory: Territory,
        entities: EntitiesRegistry,
        n: int
    ) -> None:

        coords = TerrainQuery.random_free_coord(territory, entity_map, n)

        for coord in coords:
            WorldMotor.add_entity(
                world.territory,
                world.entity_map,
                CreatureFactory.gen_creature(position=coord, creature_type=CreatureTypes.CRAB),
                entities
            )


# =========================================================
# SIMULATION STEP
# =========================================================

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
    




class PresetExecutor:
    @staticmethod
    def execute_reproduction(preset:ReproducePreset, entities:EntitiesRegistry, buffer:ReproductiveBuffer) -> None:
        female = entities.get_creature(preset.female)
        male = entities.get_creature(preset.male)

        
        costs = ReproductiveSystem.reproduce(female, male)
        female.energy.sub(costs.female_cost)
        male.energy.sub(costs.male_cost)

        buffer.try_remove(female.id)
        buffer.try_remove(male.id)
    @staticmethod
    def execute_eat(preset:EatPreset, creature:Creature) -> None:
        MetabolismSystem.eat(creature, preset.energy)
    @staticmethod
    def execute_move(preset:MovePreset, creature:Creature, perception:Perception, coord_creature:Coord, world:World) -> None:
        best_pos = MovementSystem.best_pos(creature, perception, preset.new_coord)

        if best_pos is None:
            return None
        
        
        cost = MovementSystem.calculate_cost_to_move(perception.get(preset.new_coord).cell, perception.creature_block.cell, creature)
        if creature.energy.value < cost:
            return None
        MovementSystem.move(creature, perception, preset.new_coord, world.entity_map, world.territory)
        creature.energy.sub(cost)

    @staticmethod
    def execute_attack(preset:AttackPreset, entities:EntitiesRegistry, creature:Creature) -> None:
        cost = AttackSystem.attack(creature, entities.get_creature(preset.target))
        creature.energy.sub(cost)
    @staticmethod
    def execute_preset(preset:AttackPreset | ReproducePreset | EatPreset | MovePreset, creature:Creature, world:World, perception:Perception) -> None:
        if isinstance(preset, AttackPreset):
            PresetExecutor.execute_attack(preset,world.entities, creature)
        elif isinstance(preset, MovePreset):
            PresetExecutor.execute_move(preset, creature, perception, perception.coord, world)
        elif isinstance(preset, EatPreset):
            PresetExecutor.execute_eat(preset, creature)
        elif isinstance(preset, ReproducePreset):
            PresetExecutor.execute_reproduction(preset, world.entities, world.reproductive_buffer)

def resolve_death(creature:Creature, world:World) -> None:
    corpse = DeathSystem.generate_corpse(creature)
    WorldMotor.delete_entity(world.entity_map, creature.position,creature.id, world.entities)
    WorldMotor.add_entity(world.territory, world.entity_map, corpse, world.entities)
def born_data_to_creature(born_data:BornData, position:Coord) -> Creature:
    return CreatureFactory.gen_creature(
        position=position,
        creature_type=born_data.genome.core.id,
        genome=born_data.genome,
        initial_energy=born_data.initial_energy
    )

class RunnerCreature:
    @staticmethod
    def run_basal_metabolism(creature:Creature) -> None:
        basal_metabolism = creature.energy.limit**(1/3)*1.4 + 1
        creature.energy.sub(basal_metabolism)

    @staticmethod
    def run_uterus(creature:Creature, perception:Perception) -> Creature | None:
        if creature.pregnant:
            assert isinstance(creature.uterus, PregnantUterus)
            creature.energy.sub(creature.uterus.pregnancy_cost)
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
        creature.age.add(1)

        RunnerCreature.run_basal_metabolism(creature)
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


class Runner:
    @staticmethod
    def run(world:World):
        territory = world.territory
        entitys = world.entities
        entity_map = world.entity_map
        print(f'Time: {world.time}')

        print(' '*30, end='\n\n\n')

        ### RUN CELLS
        for cell in territory.values:
            cell.pass_time()


        ### RUN CREATURES
        for coord, id in list(entity_map.iter):
            cell = territory.get(coord)
            if id.e_type == EntityTypes.CREATURE:
                creature = entitys.get_creature(id)

                print_creature(creature.interface, coord, cell)

                RunnerCreature.run_creature(creature, world)
            elif id.e_type == EntityTypes.CORPSE:
                corpse = entitys.get_corpse(id)

                print_corpse(corpse, coord, cell)

                RunnerCorpse.run_corpse(corpse, coord, entity_map, entitys)


        
        

world = World(
    TerrainFactory.gen_terrain(Coord(10, 10), ScaleGenValues.LONG.value, TerrainFactory.gen_seed()),
    EntityMap(),
    EntitiesRegistry(),
    ReproductiveBuffer()
)

id = 'jotac'




WorldMotor.add_entity(
    world.territory,
    world.entity_map,
    CreatureFactory.gen_creature(position=Coord(0, 0),
                                 creature_type=CreatureTypes.CRAB,
                                id=id),
    world.entities    
)



Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
