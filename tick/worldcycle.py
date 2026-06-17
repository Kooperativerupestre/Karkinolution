from map.world import World
from organism.identity import EntityTypes
from tick.lifecycle import RunnerCreature, RunnerCorpse


class Runner:
    @staticmethod
    def run(world:World):
        territory = world.territory
        entitys = world.entities
        entity_map = world.entity_map

        ### RUN CELLS
        for cell in territory.values:
            cell.pass_time()


        ### RUN CREATURES
        for coord, id in list(entity_map.iter):
            cell = territory.get(coord)
            if id.e_type == EntityTypes.CREATURE:
                creature = entitys.get_creature(id)

                RunnerCreature.run_creature(creature, world)
            elif id.e_type == EntityTypes.CORPSE:
                corpse = entitys.get_corpse(id)

                RunnerCorpse.run_corpse(corpse, coord, entity_map, entitys)