from karkinolution.organism.identity import EntityTypes

from karkinolution.terrain.world import World

from karkinolution.tick.lifecycle import (
    RunnerCorpse,
    RunnerCreature,
)


class Runner:
    @staticmethod
    def run(world:World):
        territory = world.territory
        entities = world.entities
        entity_map = world.entity_map

        ### RUN CELLS
        for cell in territory.values:
            cell.pass_time()


        ### RUN CREATURES
        for coord, id in list(entity_map.iter):
            cell = territory.get(coord)
            if id.e_type == EntityTypes.CREATURE:
                creature = entities.get_creature(id)

                RunnerCreature.run_creature(creature, world)
            elif id.e_type == EntityTypes.CORPSE:
                corpse = entities.get_corpse(id)

                RunnerCorpse.run_corpse(corpse, entity_map, entities)
        world.time+=1