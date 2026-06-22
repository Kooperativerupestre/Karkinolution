from dataclasses import dataclass
from map.map import TerrainMotor, Territory, EntityMap, ScaleGenValues, TerrainFactory
from core.coord import Coord
from organism.creatures import Creature, EntitiesRegistry, Corpse
from organism.identity import Id
from systems.reproductivebuffer import ReproductiveBuffer


class Log:
    def __init__(self):
        self.log_registry:list[str] = []
    def __str__(self) -> str:
        return str(self.log_registry)
    def __repr__(self) -> str:
        return str(self.log_registry)
    def add(self, message:str):
        self.log_registry.append(message)
    def clear(self):
        self.log_registry = []

class WorldMotor:
    @staticmethod
    def add_entity(territory:Territory, entity_map:EntityMap, entity:Creature | Corpse, entities:EntitiesRegistry) -> None:
        TerrainMotor.add_entity(territory=territory, entity_map=entity_map, coord=entity.position, id=entity.id)
        entities.add(entity)
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