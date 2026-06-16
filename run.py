from map.map import Territory, EntityMap, TerrainQuery
from map.cell import TerrainTypes, gen_cell, Cell, FoodState
from core.coord import Coord
from organism.genetics import CreatureTypes
from organism.creatures import Creature, Corpse, EntitysRegistry, CreatureInterface, CreatureFactory
from organism.stats import Energy
from organism.identity import Id, EntityTypes
from map.world import WorldMotor, World
from systems.biology import DeathSystem, MetabolismSystem, ReproductiveSystem, UterusSystem, Parents
from decisions.perception import perceive, Perception
from systems.presets import AtackPreset, EatPreset, MovePreset, ReproducePreset
from decisions.actions import IntentActs
from decisions.instincts import DecideIntention, Planner, ReproductiveBuffer
from systems.physics import MovementSystem, AtackSystem, SpatialSystem
from random import choice


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
    def init_territory(territory: Territory) -> None:
        size = Coord(20, 20)

        for x in range(0, size.x):
            for y in range(0, size.y):
                territory.add(Coord(x, y), gen_cell(TerrainTypes.DIRT))




    @staticmethod
    def create_creatures(
        entity_map: EntityMap,
        territory: Territory,
        entitys: EntitysRegistry,
        n: int
    ) -> None:

        coords = TerrainQuery.random_free_coord(territory, entity_map, n)

        for coord in coords:
            WorldMotor.add_entity(
                world.territory,
                world.entity_map,
                CreatureFactory.gen_creature(position=coord, creature_type=CreatureTypes.CRAB),
                entitys
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
    def resolve_parents(A:Creature, B_id:Id, entitys:EntitysRegistry) -> Parents:
        B = entitys.get_creature(B_id)


        return ReproductiveSystem.return_parents(A, B)
    
    @staticmethod
    def chose_mate(ids:list[Id]) -> Id:
        return choice(ids)
    
    
    @staticmethod
    def mate_to_preset(parents:Parents) -> ReproducePreset:
        return ReproducePreset(parents.female, parents.male)
    


class IntentResolver:
    @staticmethod
    def cancel_invalid_intents(creature:Creature) -> None:
        intent = creature.intent.intent
        intent_time = creature.intent.time
        if creature.hungry < creature.genome.metabolism.max_hungry and intent_time > 2 and intent == IntentActs.FIND_FOOD:
            creature.intent.intent = IntentActs.NOTHING

        if intent_time > 5 and intent ==  IntentActs.FIND_MATCH:
            creature.intent.intent = IntentActs.NOTHING
    @staticmethod
    def transform_to_preset(creature:Creature, perception:Perception) -> MovePreset | EatPreset | AtackPreset | None:
        if creature.intent.intent == IntentActs.FIND_FOOD:
            act = Planner.plan_find_food_intent(perception, creature)
            return act
        
        elif creature.intent.intent == IntentActs.FIND_MATCH:
            act = Planner.plan_find_match_intent(perception)
            return act

    @staticmethod
    def update_intent(creature:Creature, reproductive_buffer:ReproductiveBuffer) -> (
        MovePreset | EatPreset | AtackPreset | ReproducePreset | None):

        if creature.intent.intent == IntentActs.NOTHING:
            creature.intent = DecideIntention.decide(creature, reproductive_buffer)
    @staticmethod
    def resolve_intent(creature:Creature, reproductive_buffer:ReproductiveBuffer, perception:Perception) -> MovePreset | AtackPreset | EatPreset | None:
        IntentResolver.cancel_invalid_intents(creature)
        IntentResolver.update_intent(creature, reproductive_buffer)
        return IntentResolver.transform_to_preset(creature, perception)

class PresetExecutor:
    @staticmethod
    def execute_reproduction(preset:ReproducePreset, entitys:EntitysRegistry, buffer:ReproductiveBuffer) -> None:
        female = entitys.get_creature(preset.female)
        male = entitys.get_creature(preset.male)

        
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
        
        cost = MovementSystem.move(creature, best_pos, world.entity_map, world.territory)
        creature.energy.sub(cost)
    @staticmethod
    def execute_atack(preset:AtackPreset, entitys:EntitysRegistry, creature:Creature) -> None:
        cost = AtackSystem.atack(creature, entitys.get_creature(preset.target))
        creature.energy.sub(cost)
    @staticmethod
    def execute_preset(preset:AtackPreset | ReproducePreset | EatPreset | MovePreset, creature:Creature, world:World, perception:Perception) -> None:
        if isinstance(preset, AtackPreset):
            PresetExecutor.execute_atack(preset, world.entitys, creature)
        elif isinstance(preset, MovePreset):
            PresetExecutor.execute_move(preset, creature, perception, perception.coord, world)
        elif isinstance(preset, EatPreset):
            PresetExecutor.execute_eat(preset, creature)
        elif isinstance(preset, ReproducePreset):
            PresetExecutor.execute_reproduction(preset, world.entitys, world.reproductive_buffer)

def resolve_death(creature:Creature, world:World, coord_creature:Coord) -> None:
    corpse = DeathSystem.generate_corpse(creature)
    WorldMotor.delete_entity(world.entity_map, coord_creature, creature.id, world.entitys)
    WorldMotor.add_entity(world.territory, world.entity_map, corpse, world.entitys)

class Runner:
    # CREATURE


    @staticmethod
    def run_fisiology(creature:Creature, perception:Perception, dt: int, world:World) -> None:
        creature.age.add(dt)
        
        basal_metabolism = creature.energy.limit**(1/3)*1.4 + 1
        creature.energy.sub(basal_metabolism * dt)

        creature.intent.time += 1
        if creature.pregnant:
            assert creature.uterus is not None
            assert creature.uterus.gestation is not None
            creature.energy.sub(creature.uterus.pregnancy_cost) # type: ignore
            UterusSystem.pass_time(creature.uterus) # type: ignore

            if creature.uterus.gestation.is_ready_to_born:
                four = perception.neighbors_4_require
                possibilities = [c for c, b in four if b is not None and SpatialSystem.can_move(b, creature.genome.core.capabilities)]

                if len(possibilities) > 0:
                    new_coord = choice(possibilities)
                    ReproductiveSystem.to_birth(creature, new_coord)
    @staticmethod
    def run_creature(creature: Creature, coord_creature:Coord, world:World) -> None:    
        # ALIAS
        entity_map = world.entity_map
        territory = world.territory
        entitys = world.entitys
        # CODE
        is_dead = DeathSystem.is_dead(creature)
        if is_dead:
            resolve_death(creature, world, coord_creature)
            return None

        perception = perceive(
            creature,
            territory,
            entity_map, 
            coord_creature,
            entitys
        )
        Runner.run_fisiology(creature, perception, 1, world)
        preset = IntentResolver.resolve_intent(creature, world.reproductive_buffer, perception)
        if preset is not None:
            PresetExecutor.execute_preset(preset, creature, world, perception)



    
    # CORPSE
    
    @staticmethod
    def to_degrade_corpse(corpse:Corpse) -> None:
        corpse.energy.mul(0.95 - corpse.decomposition_time.value/100)
        corpse.decomposition_time.add(1)
    @staticmethod
    def run_corpse(corpse:Corpse, coord_corpse:Coord, entity_map:EntityMap, entitys:EntitysRegistry) -> None:
        if corpse.ready_to_disapear:
            WorldMotor.delete_entity(entity_map, coord_corpse, corpse.id, entitys)
        Runner.to_degrade_corpse(corpse)

            
    @staticmethod
    def run(world:World):
        territory = world.territory
        entitys = world.entitys
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

                Runner.run_creature(creature, coord, world)
            elif id.e_type == EntityTypes.CORPSE:
                corpse = entitys.get_corpse(id)

                print_corpse(corpse, coord, cell)

                Runner.run_corpse(corpse, coord, entity_map, entitys)


        
        


world = World(
    Territory(),
    EntityMap(),
    EntitysRegistry(),
    ReproductiveBuffer()
)

id = 'jotac'

Init.init_territory(world.territory)


WorldMotor.add_entity(
    world.territory,
    world.entity_map,
    CreatureFactory.gen_creature(position=Coord(0, 0),
                                 creature_type=CreatureTypes.CRAB,
                                id=id),
    world.entitys    
)




Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
