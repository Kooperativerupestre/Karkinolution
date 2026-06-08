from map import Territory, EntityMap, TerrainMotor, TerrainView
from cell import TerrainTypes, gen_cell
from coord import Coord
from organism.genetics import CreatureTypes, creatures_genomes
from organism.creatures import Creature, Corpse, EntitysRegistry
from organism.ontology import Gender
from namegenerator import gen_name
from identity import Id, gen_id, EntityTypes
from world import WorldMotor, World
from systems.biology import DeathSystem, MetabolismSystem
from perception import perceive
from systems.pressets import AtackPressets, EatPressets, MovePressets
from actions import IntentActs, Intent
from instincts import DecideIntention, Planner
from systems.physics import MovementSystem, AtackSystem
# =========================================================
# INIT WORLD
# =========================================================

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

    creature.energy.sub(creature.energy.limit**(1/3))
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
            print('abaixo!')
            creature.intent = Intent(IntentActs.NOTHING)
            return

        act = Planner.plan_find_food_intent(perception, creature)
        print(f'Comida próxima {MetabolismSystem.near_food(perception, creature)}')

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


world = World(
    Territory(),
    EntityMap(),
    EntitysRegistry()
)

id = Id('jotac', EntityTypes.CREATURE)

Init.init_territory(world.territory)

WorldMotor.add_entity(world.territory, world.entity_map, Init.random_creature(id), Coord(0, 0), world.entitys)

def print_creature(id:Id, entity_map:EntityMap, entitys:EntitysRegistry) -> None:
    creature = entitys.get_creature(id)


    print('Nome: {}     | ID {} |   Energy value {}/{}'.format(creature.name, creature.id, creature.energy.value, creature.energy.limit))
    print('Intent: {}   | life {}/{}'.format(creature.intent.intent.name, creature.life.value, creature.life.limit))
    print('Hungry: ', creature.hungry)
    print('-'*30, end='\n\n\n')
    print(f'Coord creature {TerrainView.get_coord_by_id(id, entity_map)}')

def run(territory:Territory, entity_map:EntityMap, entitys:EntitysRegistry):

    print(f'Tempo {world.time}')
    
    print_creature(id, world.entity_map, world.entitys)

    DecideIntention.decide(world.entitys.get_creature(id))

    run_creature(entitys.get_creature(id), TerrainView.get_coord_by_id(id, world.entity_map), territory, entity_map, entitys)


run(world.territory, world.entity_map, world.entitys)