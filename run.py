from map.map import Territory, EntityMap, TerrainQuery, TerrainFactory, ScaleGenValues
from map.cell import Cell, FoodState
from core.coord import Coord
from organism.genetics import CreatureTypes
from organism.creatures import Corpse, EntitiesRegistry, CreatureInterface, CreatureFactory
from organism.stats import Energy
from organism.identity import EntityTypes
from map.world import WorldMotor, World
from decisions.instincts import ReproductiveBuffer
from tick.lifecycle import RunnerCorpse, RunnerCreature


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
