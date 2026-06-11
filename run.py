from core.map import Territory, EntityMap, TerrainMotor, TerrainView
from core.cell import TerrainTypes, gen_cell, Cell, FoodState
from core.coord import Coord
from organism.genetics import CreatureTypes, creatures_genomes
from organism.creatures import Creature, Corpse, EntitysRegistry, CreatureInterface
from organism.ontology import Gender
from organism.stats import check_energy, Energy
from utils.namegenerator import gen_name
from organism.identity import Id, gen_id, EntityTypes
from core.world import WorldMotor, World
from systems.biology import DeathSystem, MetabolismSystem, ReproductiveSystem, UterusSystem
from decisions.perception import perceive, Perception
from systems.presets import AtackPreset, EatPreset, MovePreset, ReproducePreset
from decisions.actions import IntentActs, Intent
from decisions.instincts import DecideIntention, Planner
from systems.physics import MovementSystem, AtackSystem


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
                TerrainMotor.add_coord(
                    Coord(x, y),
                    gen_cell(TerrainTypes.DIRT),
                    territory
                )

    @staticmethod
    def random_creature(id:Id | None  = None) -> Creature:
        genome = creatures_genomes[CreatureTypes.CRAB]

        return Creature(
            genome=genome,
            gender=Gender.choice(),
            name=gen_name(),
            initial_energy=None,
            id=id.id if id is not None else gen_id()
        )

    @staticmethod
    def create_creatures(
        entity_map: EntityMap,
        territory: Territory,
        entitys: EntitysRegistry,
        n: int
    ) -> None:

        coords = TerrainView.random_free_coord(territory, entity_map, n)

        for i in range(n):
            creature = Init.random_creature()

            WorldMotor.add_entity(
                world.territory,
                world.entity_map,
                Init.random_creature(id),
                coords[n],
                entitys
            )


# =========================================================
# SIMULATION STEP
# =========================================================
class Runner:
    # CREATURE
    @staticmethod
    def update_intent(creature:Creature, perception:Perception) -> (
        MovePreset | EatPreset | AtackPreset | ReproducePreset | None):

        if creature.intent.intent == IntentActs.NOTHING:
            creature.intent = DecideIntention.decide(creature)
        
        intent = creature.intent.intent
        intent_time = creature.intent.time
        

        if creature.hungry < creature.genome.max_hungry and intent_time > 2 and intent == IntentActs.FIND_FOOD:
            creature.intent.intent = IntentActs.NOTHING

        if intent_time > 5 and  IntentActs.FIND_MATCH:
            creature.intent.intent = IntentActs.NOTHING
        
        
        if intent == IntentActs.FIND_FOOD:
            act = Planner.plan_find_food_intent(perception, creature)
            return act
        
        elif intent == IntentActs.FIND_MATCH:
            act = Planner.plan_find_match_intent(perception, creature)
            return act
    @staticmethod
    def run_fisiology(creature:Creature, dt: int) -> bool:
        creature.age.add(dt)
        
        basal_metabolism = creature.energy.limit**(1/3)*1.4 + 1
        creature.energy.sub(basal_metabolism * dt)

        creature.intent.time += 1
        if creature.pregnant:
            creature.energy.sub(creature.uterus.pregnancy_cost) # type: ignore
            UterusSystem.pass_time(creature.uterus) # type: ignore


    
        return DeathSystem.is_dead(creature)
    


    @staticmethod
    def run_creature(creature: Creature, coord_creature:Coord, territory:Territory, entity_map:EntityMap, entitys:EntitysRegistry) -> None:    
        is_dead = Runner.run_fisiology(creature, 1)

        if is_dead:
            DeathSystem.resolve_death(
                creature,
                coord_creature,
                entity_map,
                territory,
                entitys
            )
            return None

        perception = perceive(
            creature,
            territory,
            entity_map, 
            coord_creature,
            entitys
        )

        preset = Runner.update_intent(creature, perception)


        if isinstance(preset, EatPreset):
            MetabolismSystem.eat(creature, preset.energy)
        elif isinstance(preset, MovePreset):
            check_energy(creature.energy, 1.4)
            MovementSystem.move(creature, coord_creature, preset.new_coord, entity_map, territory)
            creature.energy.sub(1.4)
        elif isinstance(preset, ReproducePreset):
            pass
            # Not implemented yet
        elif isinstance(preset, AtackPreset):
            AtackSystem.atack(creature, entitys.get_creature(preset.target))
    
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
        for cell in territory.cells:
            cell.pass_time()


        ### RUN CREATURES
        for coord, id in list(entity_map.iter):
            cell = TerrainView.get_cell_by_coord(coord, territory)
            if id.e_type == EntityTypes.CREATURE:
                creature = entitys.get_creature(id)

                print_creature(creature.interface, coord, cell)

                Runner.run_creature(creature, coord, territory, entity_map, entitys)
            elif id.e_type == EntityTypes.CORPSE:
                corpse = entitys.get_corpse(id)

                print_corpse(corpse, coord, cell)

                Runner.run_corpse(corpse, coord, entity_map, entitys)


        
        


world = World(
    Territory(),
    EntityMap(),
    EntitysRegistry()
)

id = Id('jotac', EntityTypes.CREATURE)

Init.init_territory(world.territory)

WorldMotor.add_entity(world.territory, world.entity_map, Init.random_creature(id), Coord(0, 0), world.entitys)




Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
Runner.run(world)
