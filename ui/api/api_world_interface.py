from map.world import World, WorldPresets, WorldMotor
from tick.worldcycle import Runner
from map.map import TerrainQuery
from organism.creatures import CreatureFactory
from ui.interfaces import SimplyCreatureInterface, FullCreatureInterface, CellInterface, InterfaceFactory
from organism.identity import Id, EntityTypes
from core.coord import Coord

class APIWorld:
    @staticmethod
    def run_1_time(world:World) -> None:
        Runner.run(world)
    @staticmethod
    def run_5_time(world:World) -> None:
        for _ in range(0, 5):
            Runner.run(world)
    @staticmethod
    def reset_with_a_preset(entry:str) -> World:
        if entry == '1':
            return WorldPresets.crab_chaos()
        elif entry == '2':
            return WorldPresets.paranoic()
        elif entry == '3':
            return WorldPresets.titanic()
        elif entry == '4':
            return WorldPresets.normal()
        raise ValueError("Not valid option")
    @staticmethod
    def create_creature(name:str, world:World) -> bool:
        free_coords = TerrainQuery.random_free_coord(world.territory, world.entity_map, 1)
        if len(free_coords) == 0:
            return False
        free_coord = free_coords[0]
        creature = CreatureFactory.gen_creature(position=free_coord, name=name)
        WorldMotor.add_entity(world, creature)
        return True
    @staticmethod
    def get_creature_simply_interface(id:str, world:World) -> SimplyCreatureInterface | None:
        try:
            creature = InterfaceFactory.create_simply_creature_interface(world.entities.get_creature(Id(id, EntityTypes.CREATURE)))
            return creature
        except Exception:
            return None
    @staticmethod
    def get_creature_full_interface(id:str, world:World) -> FullCreatureInterface | None:
        try:
            creature = InterfaceFactory.create_full_creature_interface(world.entities.get_creature(Id(id, EntityTypes.CREATURE)))
            return creature
        except Exception:
            return None
    @staticmethod
    def get_cell_interface(coord:Coord, world:World) -> CellInterface | None:
        try:
            cell = World.territory.get(coord)
        except Exception:
            return None
        return InterfaceFactory.create_cell_interface(cell)
    @staticmethod
    def kill_creature(id_s:str, world:World) -> bool:
        id = Id(id_s, EntityTypes.CREATURE)
        try:
            WorldMotor.delete_entity(world.entity_map, id, world.entities)
            return True
        except Exception:
            return False

