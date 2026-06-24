from dataclasses import dataclass
from map.map import TerrainMotor, Territory, EntityMap, ScaleGenValues, TerrainFactory, TerrainQuery
from core.coord import Coord
from organism.creatures import Creature, EntitiesRegistry, Corpse, CreatureFactory
from organism.identity import Id
from systems.reproductivebuffer import ReproductiveBuffer

@dataclass(frozen=True)
class LogEntry:
    time:int
    message:str
    def __str__(self):
        return f"{self.time}: {self.message}"

class Log:
    def __init__(self):
        self.log_registry:list[LogEntry] = []
    def __str__(self) -> str:
        return str(self.log_registry)
    def __repr__(self) -> str:
        return str(self.log_registry)
    def add(self, log_entry:LogEntry):
        self.log_registry.append(log_entry)
    def get_by_time(self, time:int) -> list[LogEntry]:
        # O(n)
        log_entries = [l for l in self.log_registry if l.time == time]
        return log_entries
    def clear(self):
        self.log_registry = []

class WorldMotor:
    @staticmethod
    def add_entity(world:World, entity:Corpse | Creature) -> None:
        TerrainMotor.add_entity(territory=world.territory, entity_map=world.entity_map, coord=entity.position, id=entity.id)
        world.entities.add(entity)
    @staticmethod
    def add_random_creatures(world:World, k:int) -> None:
        coords = TerrainQuery.random_free_coord(world.territory, world.entity_map, k)

        for _ in range(k):
            random_creature = CreatureFactory.gen_creature(position=coords.pop())
            WorldMotor.add_entity(world, random_creature)
    @staticmethod
    def delete_entity_by_id(entity_map:EntityMap, id:Id, entities:EntitiesRegistry) -> None:
        entity_map.delete(entity_map.get_key_by_value(id)) # O(n)
        entities.delete(id)


        # Slow path O(n)
        # Use only creature's coordinate is unknow
        # Prefer delete_entity whenever possible
    @staticmethod
    def delete_entity(entity_map:EntityMap, coord:Coord, id:Id, entities:EntitiesRegistry) -> None:
        entity_map.delete(coord) # O(1)
        entities.delete(id)

@dataclass(frozen=True)
class PresetWorld:
    seed:int
    size:Coord
    scale:ScaleGenValues



@dataclass
class World:
    territory: Territory
    entity_map: EntityMap
    entities: EntitiesRegistry
    reproductive_buffer:ReproductiveBuffer
    log:Log
    time:int = 0

class WorldFactory:
    @staticmethod
    def create_world(preset:PresetWorld) -> World:
        return World(
            TerrainFactory.gen_terrain(preset.size, preset.scale.value, preset.seed),
            EntityMap(),
            EntitiesRegistry(),
            ReproductiveBuffer(),
            Log()
        )