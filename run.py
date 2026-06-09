from core.map import Territory, EntityMap, TerrainMotor, TerrainView
from core.cell import TerrainTypes, gen_cell
from core.coord import Coord
from organism.genetics import CreatureTypes, creatures_genomes
from organism.creatures import Creature, Corpse, EntitysRegistry, CreatureInterface
from organism.ontology import Gender
from utils.namegenerator import gen_name
from organism.identity import Id, gen_id, EntityTypes
from core.world import WorldMotor, World
from systems.biology import DeathSystem, MetabolismSystem
from decisions.perception import perceive
from systems.pressets import AtackPressets, EatPressets, MovePressets
from decisions.actions import IntentActs, Intent
from decisions.instincts import DecideIntention, Planner
from systems.physics import MovementSystem, AtackSystem


# =========================================================
# INIT WORLD
# =========================================================


def print_creature(creature:CreatureInterface, coord:Coord) -> None:


    print('Nome: {}     | ID {} |   Energy value {}/{}'.format(creature.name, creature.id, creature.energy.value, creature.energy.limit))
    print('Intent: {}   | life {}/{}'.format(creature.intent.intent.name, creature.life.value, creature.life.limit))
    print('Hungry: ', creature.hungry)
    print('-'*30, end='\n\n\n')
    print(f'Coord creature {coord}')

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
    @staticmethod
    def run_creature(creature: Creature, coord_creature:Coord, territory:Territory, entity_map:EntityMap, entitys:EntitysRegistry) -> None:    
        if DeathSystem.is_dead(creature):
            DeathSystem.resolve_death(
                creature,
                coord_creature,
                entity_map,
                territory,
                entitys
            )
            return

        creature.energy.sub(creature.energy.limit**(1/3)*1.4)
        creature.age.add(1)

        perception = perceive(
            creature,
            territory,
            entity_map, 
            coord_creature,
            entitys
        )

        creature.intent.time+=1

        if creature.intent.intent is IntentActs.NOTHING:
            creature.intent = DecideIntention.decide(creature)
        
        if creature.intent.intent is IntentActs.FIND_FOOD:
            if creature.hungry < creature.genome.max_hungry:
                creature.intent = Intent(IntentActs.NOTHING)
                return

            act = Planner.plan_find_food_intent(perception, creature)


            if isinstance(act, MovePressets):
                MovementSystem.move(
                    creature,
                    coord_creature,
                    act.new_coord,
                    entity_map,
                    territory
                )

            elif isinstance(act, EatPressets):
                MetabolismSystem.eat(creature, act.energy)

            elif isinstance(act, AtackPressets):
                AtackSystem.atack(act.atacker, act.target)
        if creature.intent.intent is IntentActs.FIND_MATCH:
            if creature.intent.time > 3:
                creature.intent = Intent(IntentActs.NOTHING)

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
        for coord, id in entity_map.iter:
            creature = entitys.get_creature(id)

            print_creature(creature.interface, coord)

            Runner.run_creature(creature, coord, territory, entity_map, entitys)


        
        


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